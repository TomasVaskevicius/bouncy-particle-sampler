#pragma once

#include "bouncy_particle_sampler/bps_utils.h"

namespace bps {

template<typename T, int Dim>
std::unique_ptr<BpsState<T, Dim>> NewtonianBounceOperator<T, Dim>
    ::getStateAfterBounce(
        const BpsState<T, Dim>& currentState,
        const std::function<Eigen::Matrix<T, Dim, 1>(Eigen::Matrix<T, Dim, 1>)>&
            energyGradient) const {

  auto energyGradientAtPosition = energyGradient(currentState.getLocation());
  auto velocity = currentState.getVelocity();
  auto newVelocity = velocity
                     - energyGradientAtPosition
                     * 2 * velocity.dot(energyGradientAtPosition)
                     / energyGradientAtPosition.squaredNorm();

  return BpsUtils<T, Dim>::createBpsState(
      currentState.getLocation(), newVelocity, currentState.getLastEventTime());
}

}
