#pragma once

#include <type_traits>
#include <vector>

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

  template<int N>
  using RealVector = Eigen::Matrix<RealType, N, 1>;

  using DynamicRealVector = Eigen::Matrix<RealType, Eigen::Dynamic, 1>;

  static_assert(
    std::is_floating_point<RealType_t>::value,
    "RealType template parameter in PositionAndVelocityState must be of a "
    "floating point type.");

  static_assert(
    Dimension % 2 == 0,
    "StateWithPositionAndVelocity must have dimension divisible by 2.");

  PositionAndVelocityState() = default;

  /**
   * A constructor which initialises this state at the given position and
   * velocity.
   */
  PositionAndVelocityState(
    RealVector<Dimension / 2> position, RealVector<Dimension / 2> velocity);

  /**
   * Returns an element of the state at a given index.
   * Poision is indexed from 0 to Dimension/2 - 1.
   * Velocity is indexed from Dimension/2 to Dimension - 1.
   */
  RealType getElementAtIndex(int index) const;

  /**
   * Returns a subvector of this state for the given indices vector.
   */
  DynamicRealVector getSubvector(std::vector<int> ids) const;

  /**
   * Constructs a new state, by modifying current states positions in
   * the given indices with the given modification vector.
   *
   * @ids
   *   Positions of the current state, which should be modified.
   * @modification
   *   Modifications, for the specified positions.
   * @return
   *   A new state object with the specified modification.
   */
  template<class VectorType>
  PositionAndVelocityState constructStateWithModifiedVariables(
    std::vector<int> ids, VectorType modification) const;

  RealVector<Dimension / 2> position;
  RealVector<Dimension / 2> velocity;
};

template<typename RealType>
using DynamicPositionAndVelocityState = PositionAndVelocityState<RealType, -2>;

/**
 * The comparison function for states of the above type.
 */
template<typename RealType, int Dimension>
bool operator==(
  const PositionAndVelocityState<RealType, Dimension>& lhs,
  const PositionAndVelocityState<RealType, Dimension>& rhs);

}

#include "position_and_velocity_state.tcc"
