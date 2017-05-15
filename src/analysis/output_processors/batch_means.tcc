#pragma once

#include <cmath>
#include <limits>

#include "gsl_wrappers/integration_wrappers.h"

namespace pdmp {
namespace analysis {

namespace {

template<typename RealType>
RealType getTargetLengthForTheLastBatch(
  const std::vector<Batch<RealType>>& batches) {

  if (batches.size() > 1) {
    // Get penultimate batch length.
    return batches[batches.size() - 2].trajectoryLength;
  } else {
    // We are working on the first batch.
    return 1.0f;
  }
}

template<typename RealType>
bool areEqual(RealType lhs, RealType rhs) {
  return std::fabs(lhs - rhs) < 1e-7;
}

}

template<class Pdmp, class State, class Flow>
BatchMeans<Pdmp, State, Flow>::BatchMeans(
  std::function<RealType(State)> integrand)
  : integrand_(integrand) {
}

template<class Pdmp, class State, class Flow>
auto BatchMeans<Pdmp, State, Flow>::estimateAsymptoticVariance() {
  return estimateAsymptoticVariance(estimateMean());
}

template<class Pdmp, class State, class Flow>
auto BatchMeans<Pdmp, State, Flow>::estimateAsymptoticVariance(
  RealType knownMean) {

  RealType asymptoticVariance = 0.0f;
  int numberOfTerms = 0;
  for (const auto& batch : batches_) {
    if (batch.trajectoryLength < (batches_.at(0).trajectoryLength / 4.0f)) {
      // This batch is too small to include into the final estimate.
      continue;
    }
    numberOfTerms++;
    asymptoticVariance +=
      (batch.value / sqrt(batch.trajectoryLength))
      - (knownMean * sqrt(batch.trajectoryLength));
  }
  return asymptoticVariance / (numberOfTerms - 1);
}

template<class Pdmp, class State, class Flow>
auto BatchMeans<Pdmp, State, Flow>::getBatches() {
  return batches_;
}

template<class Pdmp, class State, class Flow>
auto BatchMeans<Pdmp, State, Flow>::estimateMean() {
  RealType totalValue = 0.0f;
  for (const auto& batch : batches_) {
    totalValue += batch.value;
  }
  return totalValue / totalTrajectoryLength_;
}

template<class Pdmp, class State, class Flow>
void BatchMeans<Pdmp, State, Flow>::processTwoSequentialResults(
  const IterationResult<State>& first,
  const IterationResult<State>& second) {

  auto segmentLength = second.iterationTime;
  auto lastBatchLength = this->batches_.back().trajectoryLength;
  decltype(lastBatchLength) targetLength = getTargetLengthForTheLastBatch(
    this->batches_);

  if (lastBatchLength + segmentLength > targetLength) {
    // Only a part of this segment calculation goes into the last batch.
    auto takenLength = targetLength - lastBatchLength;
    auto midResult = IterationResult<State>(
      Flow::advanceStateByFlow(first.state, takenLength),  takenLength);
    auto newEndResult = IterationResult<State>(
      second.state, segmentLength - takenLength);

    // Handle the two segments separately.
    processTwoSequentialResults(first, midResult);
    processTwoSequentialResults(midResult, newEndResult);
  } else {
    // All of this segment goes into the last batch.
    batches_.back().trajectoryLength += segmentLength;
    batches_.back().value += bps::GslIntegrationWrappers<RealType>::integrate(
      [&integrand_ = integrand_, &first] (RealType t) -> RealType {
        return integrand_(Flow::advanceStateByFlow(first.state, t));
      },
      0.0f,
      segmentLength);

    // We now need to handle the creation/merging of batches.
    handleBatchRearrangements();
    totalTrajectoryLength_ += segmentLength;
  }
};

template<class Pdmp, class State, class Flow>
void BatchMeans<Pdmp, State, Flow>::handleBatchRearrangements() {
  // We handle the batch rearrangements in accordance to the datatype invariant
  // documented in batc_means.h.
  auto lastBatchLength = batches_.back().trajectoryLength;
  decltype(lastBatchLength) targetLength = getTargetLengthForTheLastBatch(
    this->batches_);

  // First check if the last batch is saturated.
  if (areEqual(lastBatchLength, targetLength)) {
    if (batches_.size() > lastBatchLength) {
      // We need to merge two batches as we already have too many.
      // Our aim is to have sqrt(T) batches for trajectory length of T.
      mergeTwoBatches();
    }
    batches_.push_back({0.0f, 0.0f});
  }
}

template<class Pdmp, class State, class Flow>
void BatchMeans<Pdmp, State, Flow>::mergeTwoBatches() {
  Batch<RealType> mergedBatch{
    batches_.at(nextMergeIndex).trajectoryLength
      + batches_.at(nextMergeIndex + 1).trajectoryLength,
    batches_.at(nextMergeIndex).value
      + batches_.at(nextMergeIndex + 1).value};
  batches_.at(nextMergeIndex + 1) = mergedBatch;
  batches_.erase(batches_.begin() + nextMergeIndex);
  nextMergeIndex++;
  if (nextMergeIndex == batches_.size()) {
    // We have wrapped around and should start a new cycle.
    nextMergeIndex = 0;
  }
}

}
}
