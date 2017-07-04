#pragma once

#include <type_traits>

namespace pdmp {

namespace {

template<class State>
struct AdvanceStateHelper {

  template<typename RealType>
  static State advanceStateByFlow(State&& state, RealType time) {
    state.position += state.velocity * time;
    return std::move(state);
  }

  template<typename RealType>
  static State advanceStateByFlow(const State& state, RealType time) {
    return State(state.position + state.velocity * time, state.velocity);
  }

};

}

template<class State, typename RealType>
std::decay_t<State> LinearFlow
  ::advanceStateByFlow(State&& state, RealType time) {

  using State_t = std::decay_t<State>;
  return AdvanceStateHelper<State_t>::advanceStateByFlow(
    std::forward<State>(state), time);
}

}
