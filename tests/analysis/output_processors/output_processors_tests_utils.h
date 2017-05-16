#pragma once

namespace myutils {

struct State {
  using RealType = float;
  float x;
};

struct SimpleFlow {
  static State advanceStateByFlow(State state, float time) {
    return State{state.x + time};
  }
};

bool areEqual(float lhs, float rhs, float precision = 1e-5) {
  return std::fabs(lhs - rhs) < precision;
}

}
