#pragma once

namespace pdmp {

template<class State, typename RealType>
State LinearFlow::advanceStateByFlow(State state, RealType time) {
  auto position = state.position;
  auto velocity = state.velocity;
  return State(position + velocity * time, velocity);
}

}
