#pragma once

#include <type_traits>
#include <vector>

namespace pdmp {

/**
 * A class for representing constant velocity flow.
 */
class LinearFlow {

 public:

  /**
   * For a given state (position, velocity) and time t, calculates and returns
   * (position + velocity * t, velocity).
   */
  template<class State, typename RealType>
  static std::decay_t<State> advanceStateByFlow(State&& state, RealType time);

  /**
   * Returns variables, dependent on a given variable id for a given state
   * space dimensionality. In particular, position variables depend on
   * associated velocity variables.
   */
  static std::vector<int> getDependentVariableIds(int variableId, int dim);

};

}

#include "linear_flow.tcc"
