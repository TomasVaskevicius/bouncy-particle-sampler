#pragma once

#include <type_traits>
#include <vector>

#include "core/policies/linear_flow_base.h"

namespace pdmp {

/**
 * A class for representing constant velocity flow.
 */
class LinearFlow : public LinearFlowBase<LinearFlow> {

 public:

  /**
   * For a given state (position, velocity) and time t, calculates and returns
   * (position + velocity * t, velocity).
   */
  template<class State, typename RealType>
  static std::decay_t<State> advanceStateByFlow(State&& state, RealType time);

};

}

#include "linear_flow.tcc"
