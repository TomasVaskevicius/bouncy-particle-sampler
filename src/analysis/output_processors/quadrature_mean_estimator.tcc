#pragma once

#include "gsl_wrappers/integration_wrappers.h"

namespace pdmp {
namespace analysis {

template<class Pdmp, class State, class Flow>
QuadratureMeanEstimator<Pdmp, State, Flow>::QuadratureMeanEstimator(
  std::function<RealType(State)> integrand)
  : integrand_(integrand) {
}

template<class Pdmp, class State, class Flow>
auto QuadratureMeanEstimator<Pdmp, State, Flow>::estimateMean() {
  return currentIntegratedPathValue_ / totalTrajectoryLength_;
}

template<class Pdmp, class State, class Flow>
void QuadratureMeanEstimator<Pdmp, State, Flow>::processTwoSequentialResults(
  const IterationResult<State>& first,
  const IterationResult<State>& second) {

  totalTrajectoryLength_ += second.iterationTime;
  currentIntegratedPathValue_ +=
    bps::GslIntegrationWrappers<RealType>::integrate(
      [this, &first] (RealType t) -> RealType {
        return this->integrand_(Flow::advanceStateByFlow(first.state, t));
      },
      0.0f,
      second.iterationTime);
}

}
}
