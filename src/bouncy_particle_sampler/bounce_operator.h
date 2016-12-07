#pragma once

#include "bouncy_particle_sampler/bps_state.h"

#include <Eigen/Core>
#include <memory>

namespace bps {

/**
 * An abstract class for representing the bounce operator used
 * by the Bouncy Particle Sampler algorithm.
 */
template<typename FloatingPointType, int Dimensionality>
class BounceOperator {

 public:

  ~BounceOperator() {}

  /**
   * Returns a new state given the current state by performing the bounce.
   */
  virtual std::unique_ptr<BpsState<FloatingPointType, Dimensionality>>
    getStateAfterBounce(
      const BpsState<FloatingPointType, Dimensionality>& currentState,
      const std::function<
          Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
          Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>&
          energyGradient) const = 0;

};

}
