#pragma once

namespace bps {

template<typename T, int Dim>
BpsState<T, Dim> BpsLinearFlow<T, Dim>::advanceStateByFlowFunction(
      const BpsState<T, Dim>& initialState,
      const T& time) const {

  auto initialPosition = initialState.getLocation();
  auto velocity = initialState.getVelocity();
  auto newPosition = initialPosition + velocity * time;
  return BpsState<T, Dim>(
    newPosition, velocity, initialState.getLastEventTime());
}

}
