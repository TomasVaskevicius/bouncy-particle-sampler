#pragma once

#include "bouncy_particle_sampler/bps_utils.h"
#include "bouncy_particle_sampler/estimators/integrated_autocorrelation_time.h"

#include <algorithm>

namespace {

template<typename T, int Dim>
T calculateAutocorrelation(
    const typename bps::Mcmc<T, Dim>::SampleOutput& sampleRun,
    const typename bps::Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
    const int& lag,
    const T& mean,
    const T& variance) {

  if (lag == 0) {
    // We add this special case to avoid calculation errors for this simple
    // case because we do not have the true variance.
    return (T) 1.0;
  }

  T delta = 0.25;
  int lagStepSize = 4;
  auto equallySpacedPoints = bps::BpsUtils<T, Dim>
      ::getEquallySpacedPointsFromBpsRun(sampleRun, delta);

  if (equallySpacedPoints.size() < lag + 100) {
    // We have less than 100 datapoints for estimating correlation.
    std::domain_error(
        "The trajectory length for the current bps run is too short to "
        "calculate autocorrelation with lag " + std::to_string(lag));
  }

  T covarianceEstimate = 0.0;
  for(int i = lag * lagStepSize; i < equallySpacedPoints.size(); i += 1) {
    covarianceEstimate +=
         (functionToEstimate(equallySpacedPoints[i - lag * lagStepSize]) - mean)
       * (functionToEstimate(equallySpacedPoints[i]) - mean);
  }

  covarianceEstimate /= equallySpacedPoints.size() - lag;

  return covarianceEstimate / variance;
}

template<typename T, int Dim>
std::vector<T> calculateAutocorrelationsForSampleRun(
    const typename bps::Mcmc<T, Dim>::SampleOutput& sampleRun,
    const typename bps::Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
    const int& lagUpperbound,
    const int& lagStepSize,
    const T& mean,
    const T& variance) {

  T delta = 0.25;
  int lagStepSizeMultiplier = 4;
  auto equallySpacedPoints = bps::BpsUtils<T, Dim>
      ::getEquallySpacedPointsFromBpsRun(sampleRun, delta);

  std::vector<T> lags;
  for (int lag = 0; lag <= lagUpperbound; lag += lagStepSize) {
    lags.push_back(calculateAutocorrelation(
      equallySpacedPoints,
      lagStepSize * lagStepSizeMultiplier,
      mean,
      variance));
  }

  return lags;
}

}

namespace bps {
namespace analysis {

template<typename T, int Dim>
std::vector<std::vector<T>> OutputAnalysis<T, Dim>
    ::calculateExpectationEstimates(
      const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns,
      const typename Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
      const typename OutputAnalysis<T, Dim>::ExpecatationEstimator&
        expectationEstimator) {

  std::vector<std::vector<T>> expectationEstimates;

  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  expectationEstimates.push_back(
                    expectationEstimator(sampleRun, functionToEstimate));
                });

  return expectationEstimates;
}

template<typename T, int Dim>
std::vector<T> OutputAnalysis<T, Dim>::calculateIntegratedAutocorrelationTime(
    const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns,
    const typename Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
    const typename OutputAnalysis<T, Dim>::ExpecatationEstimator&
      expectationEstimator,
    const int& numberOfBatches) {

  std::vector<T> iactEstimates;

  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  iactEstimates.push_back(
                    IntegratedAutocorrelationTime<T, Dim>
                      ::calculateIntegratedAutocorrelationTime(
                        sampleRun,
                        functionToEstimate,
                        expectationEstimator,
                        numberOfBatches));
                });

  return iactEstimates;
}

template<typename T, int Dim>
std::vector<T> OutputAnalysis<T, Dim>::calculateEffectiveSampleSizePerSecond(
  const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns,
  const std::vector<T>& integratedAutocorrelationTimes,
  const std::vector<double>& runningTimesInMilliseconds) {

  std::vector<T> effectiveSampleSizesPerSecond;
  for (int i = 0; i < sampleRuns.size(); i++) {
    T trajectoryLength = BpsUtils<T, Dim>::getTotalPathLength(sampleRuns[i]);
    T iact = integratedAutocorrelationTimes[i];
    T timeInSeconds = (T) runningTimesInMilliseconds[i] / 1000.0;

    effectiveSampleSizesPerSecond.push_back(
      trajectoryLength / (iact * timeInSeconds));
  }

  return effectiveSampleSizesPerSecond;
}

template<typename T, int Dim>
std::vector<T> OutputAnalysis<T, Dim>::calculateAverageIntereventTime(
    const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns) {

  std::vector<T> averageIntereventTimes;

  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  T totalPathLength =
                    BpsUtils<T, Dim>::getTotalPathLength(sampleRun);
                  int numberOfEvents = sampleRun.size();
                  averageIntereventTimes.push_back(
                    totalPathLength / (numberOfEvents - 1));
                });

  return averageIntereventTimes;
}

template<typename T, int Dim>
std::vector<std::vector<T>>
  OutputAnalysis<T, Dim>::calculateAutocorrelationFunction(
    const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns,
    const typename Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
    const typename OutputAnalysis<T, Dim>::ExpecatationEstimator&
      expectationEstimator,
    const int& lagUpperbound,
    const int& lagStepSize) {

  T mean = estimateMeanFromSampleRuns(
      sampleRuns, functionToEstimate, expectationEstimator);
  T variance = estimateVarianceFromSampleRuns(
      sampleRuns, functionToEstimate, expectationEstimator, mean);

  std::vector<int> lags;
  for (int i = 0; i <= lagUpperbound; i += lagStepSize) {
    lags.push_back(i);
  }

  std::vector<std::vector<T>> autocorrelations;
  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  std::vector<T> acfForCurrentSampleRun;
                  for (const int& lag : lags) {
                    acfForCurrentSampleRun.push_back(
                      calculateAutocorrelation<T, Dim>(
                        sampleRun,
                        functionToEstimate,
                        lag,
                        mean,
                        variance));
                  }
                  autocorrelations.push_back(acfForCurrentSampleRun);
                });

  return autocorrelations;
}

template<typename T, int Dim>
T OutputAnalysis<T, Dim>::estimateMeanFromSampleRuns(
       const SampleOutputsVector& sampleRuns,
       const typename Mcmc<T, Dim>
         ::RealFunctionOnSamples& functionToEstimate,
       const ExpecatationEstimator& expectationEstimator) {

  T sumOfMeanEstimates = 0.0;
  for (const auto& sampleRun : sampleRuns) {
    std::vector<T> expectationEstimates =
      expectationEstimator(sampleRun, functionToEstimate);
    sumOfMeanEstimates += expectationEstimates[expectationEstimates.size() - 1];
  }
  return sumOfMeanEstimates / sampleRuns.size();
}

template<typename T, int Dim>
T OutputAnalysis<T, Dim>::estimateVarianceFromSampleRuns(
       const SampleOutputsVector& sampleRuns,
       const typename Mcmc<T, Dim>
         ::RealFunctionOnSamples& functionToEstimate,
       const ExpecatationEstimator& expectationEstimator,
       const T& meanEstimate) {

  auto squaredFunction =
    [&functionToEstimate] (Eigen::Matrix<T, Dim, 1> vector) -> T {
      T value = functionToEstimate(vector);
      return value * value;
    };

  T meanOfSquaredFunction = estimateMeanFromSampleRuns(
      sampleRuns,
      squaredFunction,
      expectationEstimator);

  return meanOfSquaredFunction - meanEstimate * meanEstimate;
}

template<typename T, int Dim>
std::vector<std::vector<T>> OutputAnalysis<T, Dim>::getEventTimes(
    const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns) {

  std::vector<std::vector<T>> eventTimes;
  for (int i = 0; i < sampleRuns.size(); i++) {
    std::vector<T> eventTimesForIthRun;
    for (const auto& sample: sampleRuns[i]) {
      eventTimesForIthRun.push_back(
        BpsUtils<T, Dim>::getEventTimeFromMcmcState(sample));
    }
    eventTimes.push_back(eventTimesForIthRun);
  }
  return eventTimes;
}

}
}

