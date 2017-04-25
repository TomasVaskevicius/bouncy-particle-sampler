#pragma once

namespace pdmp {

template<typename RealType, int Dimension>
PositionAndVelocityState<RealType, Dimension>::PositionAndVelocityState(
  RealVector position, RealVector velocity)
  : position(position), velocity(velocity) {}


template<typename RealType, int Dimension>
bool operator==(
  const PositionAndVelocityState<RealType, Dimension>& lhs,
  const PositionAndVelocityState<RealType, Dimension>& rhs) {

  return lhs.position.isApprox(rhs.position)
         && lhs.velocity.isApprox(rhs.velocity);
}

}
