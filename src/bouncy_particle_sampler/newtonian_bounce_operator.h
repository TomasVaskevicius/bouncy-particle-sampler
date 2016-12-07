#pragma once

#include "bouncy_particle_sampler/bounce_operator.h"

#include <Eigen/Core>

namespace bps {

/**
 * Bounce operator implementation, where upon bounce, the particle
 * velocity is updated as a Newtonian elastic collision on the
 * hyperplane tangential to the gradient of the energy.
 */
template<typename FloatingPointType, int Dimensionality>
class NewtonianBounceOperator
    : public BounceOperator<FloatingPointType, Dimensionality> {

 public:

  std::unique_ptr<BpsState<FloatingPointType, Dimensionality>>
    getStateAfterBounce(
      const BpsState<FloatingPointType, Dimensionality>& currentState,
      const std::function<
          Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
          Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>&
          energyGradient) const override;

};

}

#include "bouncy_particle_sampler/newtonian_bounce_operator.tcc"
