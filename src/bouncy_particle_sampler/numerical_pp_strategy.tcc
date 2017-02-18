#pragma once

#include "bouncy_particle_sampler/linear_flow.h"
#include "poisson_process/nhpp_by_approximating_quantile.h"
#include "poisson_process/nhpp_by_thinning.h"

#include <algorithm>
#include <memory>

namespace bps {

template<typename T, int Dim>
NumericalPoissonProcessStrategy<T, Dim>::NumericalPoissonProcessStrategy(
  const EnergyGradient& energyGradient)
  : BpsPoissonProcessStrategy<T, Dim>(energyGradient) {
}

template<typename T, int Dim>
T NumericalPoissonProcessStrategy<T, Dim>::getNextEventTime(
  const BpsState<T, Dim>& bpsState) const {

  std::unique_ptr<PoissonProcess<T>> poissonProcess(
      new NonHomogeneousPoissonProcessByApproximatingQuantile<T>(
          [this, &bpsState] (T t) -> T {
            auto positionAtTimeT = this->flow_->advanceStateByFlowFunction(
              bpsState, t).getLocation();
            auto intensity = bpsState.getVelocity().dot(
              this->energyGradient_(positionAtTimeT));

            return std::max((T) 0.0, intensity);
          }));

  return poissonProcess->getNextSample()(0);
}

}
