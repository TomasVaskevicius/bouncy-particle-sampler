#pragma once

#include "bouncy_particle_sampler/bps_flow.h"
#include "bouncy_particle_sampler/bps_state.h"

#include <functional>
#include <memory>

namespace bps {

/**
 * A class responsible for generating poisson processes given
 * BpsState.
 *
 * Different models will need to provide different implementations of
 * this class for efficient inhomogeneous Poisson process simulation.
 */
template<typename FloatingPointType, int Dimensionality>
class BpsPoissonProcessStrategy {

  typedef std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
                        Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>
          EnergyGradient;
 public:

  /**
   * Only the energy gradient of the target distribution is needed.
   */
  BpsPoissonProcessStrategy(
    const EnergyGradient& energyGradient);

  ~BpsPoissonProcessStrategy() {}

  /**
   * Simulates the next poisson process arrival time for a given BpsState and
   * energy gradient function.
   */
  virtual FloatingPointType getNextEventTime(
    const BpsState<FloatingPointType, Dimensionality>& bpsState) const = 0;

 protected:

  const EnergyGradient& energyGradient_;
  const std::shared_ptr<BpsFlow<FloatingPointType, Dimensionality>> flow_;

};



}

#include "pp_strategy.tcc"
