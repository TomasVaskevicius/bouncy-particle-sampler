#pragma once

#include "analysis/output_processors/sequential_processor_base.h"

namespace pdmp {
namespace analysis {

/**
 * An expectation estimator, which simply integrates the path
 * by using Gaussian quadrature.
 */
template<class Pdmp, class State, class Flow>
class QuadratureMeanEstimator : public SequentialProcessorBase<Pdmp, State> {

 public:

  using RealType = typename State::RealType;

  /**
   * Construct the batch means processor by providing the integrand
   * of interest.
   */
  QuadratureMeanEstimator(std::function<RealType(State)> integrand);

  /**
   * Returns the mean estimate.
   */
  auto estimateMean();

 protected:

  virtual void processTwoSequentialResults(
    const IterationResult<State>& first,
    const IterationResult<State>& second) override final;

 private:

  RealType totalTrajectoryLength_{0.0f};
  RealType currentIntegratedPathValue_{0.0f};
  std::function<RealType(State)> integrand_;

};

}
}

#include "quadrature_mean_estimator.tcc"
