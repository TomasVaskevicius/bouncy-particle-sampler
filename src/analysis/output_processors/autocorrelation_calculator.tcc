#pragma once

#include <algorithm>

#include "gsl_wrappers/integration_wrappers.h"

namespace {

const int kMaxLag = 100;
const int kStepsPerUnitLength = 4;
const int kWindowSize = kMaxLag * kStepsPerUnitLength;
const float kWaitForTrajectoryLength = 1000.0f;

// A helper for calculating covariance.
// Starts from integrandAtPoints[kWindowSize].
template<class RealType>
void updateCovarianceEstimates(
  const int& whichLagStep,
  const RealType& mean,
  std::vector<RealType>& covarianceEstimate,
  const std::vector<RealType>& integrandAtPoints) {

  for (int i = kWindowSize; i < integrandAtPoints.size(); i++) {
    covarianceEstimate.at(whichLagStep) +=
      (integrandAtPoints.at(i - whichLagStep) - mean)
      * (integrandAtPoints.at(i) - mean);
  }
}

}

namespace pdmp {
namespace analysis {

template<class Pdmp, class State, class Flow>
void AutocorrelationCalculator<Pdmp, State, Flow>::TimeWindow::addSegment(
  const IterationResult<State>& start,
  const IterationResult<State>& end,
  std::function<RealType(State)>& integrand) {

  State left = start.state;
  RealType remainingLength = end.iterationTime;
  RealType stepSize = (RealType) 1.0f / kStepsPerUnitLength;

  // Deal with the remainder segment from the alst iteration.
  if (end.iterationTime + paddingSize_ < stepSize) {
    paddingSize_ += end.iterationTime;
    return;
  } else {
    left = Flow::advanceStateByFlow(left, stepSize - paddingSize_);
    integrandAtPoints_.push_back(integrand(left));
    remainingLength -= stepSize - paddingSize_;
    paddingSize_ = 0.0f;
  }

  while (remainingLength >= stepSize) {
    left = Flow::advanceStateByFlow(left, stepSize);
    integrandAtPoints_.push_back(integrand(left));
    remainingLength -= stepSize;
  }

  paddingSize_ = remainingLength;
}


template<class Pdmp, class State, class Flow>
void AutocorrelationCalculator<Pdmp, State, Flow>
  ::TimeWindow::updateCovarianceEstimatesAndCleanUpData(
    const RealType& mean,
    std::vector<RealType>& covarianceEstimates,
    int& numberOfPointsUsedForCovarianceEstimates) {

  // For each possible step size, update covariance.
  for (int i = 0; i < kWindowSize; i++) {
    updateCovarianceEstimates(
      i,
      mean,
      covarianceEstimates,
      integrandAtPoints_);
  }
  numberOfPointsUsedForCovarianceEstimates
    += std::max(0, (int) (integrandAtPoints_.size() - kWindowSize));

  // Keep the end of integrandAtPoints_ vector for next covariance update call.
  if (integrandAtPoints_.size() > kWindowSize) {
    integrandAtPoints_.erase(
      integrandAtPoints_.begin(), integrandAtPoints_.end() - kWindowSize);
  }
}

template<class Pdmp, class State, class Flow>
std::vector<typename State::RealType>
AutocorrelationCalculator<Pdmp, State, Flow>
  ::TimeWindow::getIntegrandAtPoints() const {

  return integrandAtPoints_;
}

template<class Pdmp, class State, class Flow>
AutocorrelationCalculator<Pdmp, State, Flow>::AutocorrelationCalculator(
  std::function<RealType(State)> integrand)
  : integrand_(integrand),
    hasRealMeanAndVariance_(false),
    covarianceEstimates_(kWindowSize, 0.0f) {
}

template<class Pdmp, class State, class Flow>
AutocorrelationCalculator<Pdmp, State, Flow>::AutocorrelationCalculator(
  std::function<RealType(State)> integrand,
  const RealType& realMean,
  const RealType& realVariance)
  : integrand_(integrand),
    realMean_(realMean),
    realVariance_(realVariance),
    hasRealMeanAndVariance_(true),
    covarianceEstimates_(kWindowSize, 0.0f) {
}

template<class Pdmp, class State, class Flow>
auto AutocorrelationCalculator<Pdmp, State, Flow>
  ::getAutocorrelationsVector() {

  auto mean = estimateMean();
  auto variance = estimateVariance();
  // If called befor kWaitForTrajectoryLength path length was generated
  // we need to calculate covariances now.
  timeWindow_.updateCovarianceEstimatesAndCleanUpData(
    estimateMean(),
    covarianceEstimates_,
    numberOfPointsUsedForCovarianceEstimates_);
  // Now calculate the autocorrelations.
  std::vector<int> autocorrelations;
  autocorrelations.push_back(1.0f); // For 0 lag.
  for (int i = 0; i < covarianceEstimates_.size(); i++) {
    auto correlation =
      covarianceEstimates_.at(i)
      / (variance * (numberOfPointsUsedForCovarianceEstimates_ - 1));
    autocorrelations.push_back(correlation);
  }
  return autocorrelations;
}

template<class Pdmp, class State, class Flow>
int AutocorrelationCalculator<Pdmp, State, Flow>::getMaxLag() const {
  return kMaxLag;
}

template<class Pdmp, class State, class Flow>
int AutocorrelationCalculator<Pdmp, State, Flow>::getLagStepsPerUnitLength()
  const {

  return kStepsPerUnitLength;
}

template<class Pdmp, class State, class Flow>
auto AutocorrelationCalculator<Pdmp, State, Flow>::estimateMean() const {
  if (hasRealMeanAndVariance_) {
    return realMean_;
  } else {
    return currentPathIntegral_ / totalTrajectoryLength_;
  }
}

template<class Pdmp, class State, class Flow>
auto AutocorrelationCalculator<Pdmp, State, Flow>::estimateVariance() const {
  if (hasRealMeanAndVariance_) {
    return realVariance_;
  } else {
    auto mean = currentPathIntegral_ / totalTrajectoryLength_;
    auto meanOfSquaredIntegrand =
      currentPathIntegralOfSquaredFunction_ / totalTrajectoryLength_;
    return meanOfSquaredIntegrand - mean * mean;
  }
}

template<class Pdmp, class State, class Flow>
void AutocorrelationCalculator<Pdmp, State, Flow>::processTwoSequentialResults(
  const IterationResult<State>& first,
  const IterationResult<State>& second) {

  totalTrajectoryLength_ += second.iterationTime;
  updateMeanStatistics(first, second);
  timeWindow_.addSegment(first, second, integrand_);
  if (totalTrajectoryLength_ >= kWaitForTrajectoryLength) {
    timeWindow_.updateCovarianceEstimatesAndCleanUpData(
      estimateMean(),
      covarianceEstimates_,
      numberOfPointsUsedForCovarianceEstimates_);
  }
}

template<class Pdmp, class State, class Flow>
void AutocorrelationCalculator<Pdmp, State, Flow>::updateMeanStatistics(
  const IterationResult<State>& first,
  const IterationResult<State>& second) {

  currentPathIntegral_ +=
    bps::GslIntegrationWrappers<RealType>::integrate(
      [this, &first] (RealType t) -> RealType {
        return this->integrand_(Flow::advanceStateByFlow(first.state, t));
      },
      0.0f,
      second.iterationTime);

  currentPathIntegralOfSquaredFunction_ +=
    bps::GslIntegrationWrappers<RealType>::integrate(
      [this, &first] (RealType t) -> RealType {
        RealType evaluatedFunction =
          this->integrand_(Flow::advanceStateByFlow(first.state, t));
        return evaluatedFunction * evaluatedFunction;
      },
      0.0f,
      second.iterationTime);
}

}
}
