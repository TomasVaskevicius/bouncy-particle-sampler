#pragma once

#include <functional>
#include <vector>

#include "analysis/output_processors/sequential_processor_base.h"

namespace pdmp {
namespace analysis {

/**
 * A batch representation. Holds the trajectory length and the computed value.
 */
template<class RealType>
struct Batch {
  RealType trajectoryLength;
  RealType value;
};

template<class Pdmp, class State, class Flow>
class BatchMeans : public SequentialProcessorBase<Pdmp, State> {

 public:

  using RealType = typename State::RealType;

  /**
   * Construct the batch means processor by providing the integrand
   * of interest.
   */
  BatchMeans(std::function<RealType(State)> integrand);

  /**
   * Estimates the asymptotic variance using the batch means.
   */
  auto estimateAsymptoticVariance();

  /**
   * Estimates the asymptotic variance using the batch means and
   * the known expectation of our integrand under the invariant
   * distribution of our samples.
   */
  auto estimateAsymptoticVariance(RealType knownMean);

  /**
   * Returns the computed batches.
   */
  auto getBatches();

  /**
   * Returns the mean estimate obtined by integrating the whole path and
   * rescaling by path length.
   */
  auto estimateMean();

 protected:

  virtual void processTwoSequentialResults(
    const IterationResult<State>& first,
    const IterationResult<State>& second) override final;

 private:

  // Should be called after modifying the last batch, to see if a new one
  // needs to be created and others possibly merged.
  void handleBatchRearrangements();

  // Merges two batches in accordance to the datatype invariant defined below.
  void mergeTwoBatches();

  // A helper for the mergeTwoBatches() function to avoid recomputation.
  int nextMergeIndex{0};

  // Datatype invariants:
  // 1) At any particular time, only two batch length's are allowed for all (but
  // the last batch), ant these sizes must be consecutive powers of two.
  // 2) The batches are ordered in order of decreasing length.
  // 3) The order of batches corresponds to the order of path evolution.
  std::vector<Batch<RealType>> batches_{Batch<RealType>{0.0f, 0.0f}};

  RealType totalTrajectoryLength_{0.0f};
  std::function<RealType(State)> integrand_;

};

}
}

#include "batch_means.tcc"
