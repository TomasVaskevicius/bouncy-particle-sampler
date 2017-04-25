#pragma once

#include <type_traits>

#include <Eigen/Core>

namespace pdmp {

/**
 * This class is used to represent the state-space for all
 * piecewise-deterministic Markov processes in R^{2d}, where the first
 * d variables represent the position and the last d variables represent
 * the velocity of the particle.
 */
template<typename RealType_t, int Dimension>
struct PositionAndVelocityState {

  using RealType = RealType_t;
  using RealVector = Eigen::Matrix<RealType, Dimension / 2, 1>;

  static_assert(
    std::is_floating_point<RealType_t>::value,
    "RealType template parameter in PositionAndVelocityState must be of a "
    "floating point type.");

  static_assert(
    Dimension % 2 == 0,
    "StateWithPositionAndVelocity must have dimension divisible by 2.");

  /**
   * A constructor which initialises this state at the given position and
   * velocity.
   */
  PositionAndVelocityState(RealVector position, RealVector velocity);

  const RealVector position;
  const RealVector velocity;

};

/**
 * The comparison function for states of the above type.
 */
template<typename RealType, int Dimension>
bool operator==(
  const PositionAndVelocityState<RealType, Dimension>& lhs,
  const PositionAndVelocityState<RealType, Dimension>& rhs);

}

#include "position_and_velocity_state.tcc"
