#pragma once

namespace pdmp {
namespace dependencies_grap {

template<class State>
MarkovKernelNodeBase<State>::MarkovKernelNodeBase(
  const std::vector<int>& dependentVariableIds)
  : dependentVariableIds(dependentVariableIds) {
};

template<class State, class Lambda>
MarkovKernelNode<State, Lambda>::MarkovKernelNode(
  const std::vector<int>& dependentVariableIds,
  const Lambda& lambda)
  : MarkovKernelNodeBase<State>(dependentVariableIds),
    lambda_(lambda) {
}

template<class State, class Lambda>
State MarkovKernelNode<State, Lambda>::jump(const State& state) {
  auto stateSubvector = state.getSubvector(this->dependentVariableIds);
  auto modifiedSubvector = this->lambda_(stateSubvector);
  return state.constructStateWithModifiedVariables(
    this->dependentVariableIds, modifiedSubvector);
}

}
}
