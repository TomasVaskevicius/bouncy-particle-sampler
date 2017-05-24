#pragma once

#include <stdexcept>
#include <type_traits>

namespace {

/**
 * Throws an exception if the dimension is not a positive and even number.
 */
constexpr void checkStateSpaceDimensionality(int dim) {
  (dim <= 0 || dim % 2 == 1) ?
    throw std::logic_error("ConstantVelocityFlow can only be used on state "
                           "spaces with positive and even dimensionalities.")
    : 0;
}

/**
 * Throws an exception if the given variable id is out of range.
 */
constexpr void checkVariableIdRange(int variableId, int dim) {
  (variableId < 0 || variableId >= dim) ?
    throw std::out_of_range("Variable id is out of range for dependent "
                            "variables calculation.")
    : 0;
}



}

namespace pdmp {

template<class State, typename RealType>
std::decay_t<State> LinearFlow
  ::advanceStateByFlow(State&& state, RealType time) {

  using State_t = std::decay_t<State>;
  if (std::is_rvalue_reference<decltype(state)>::value) {
    return State_t(
      std::move(state.position + state.velocity * time),
      std::move(state.velocity));
  } else {
    return State_t(state.position + state.velocity * time, state.velocity);
  }
}

std::vector<int> LinearFlow::getDependentVariableIds(int variableId, int dim) {
  checkStateSpaceDimensionality(dim);
  checkVariableIdRange(variableId, dim);
  if (variableId < dim / 2) {
    // This is a position variable. No other variable depends on this variable.
    return std::vector<int>{variableId};
  } else {
    // This is a velocity variable.
    // Return also its associated position variable.
    return std::vector<int>{variableId - dim / 2, variableId};
  }
}

}
