#pragma once

namespace pdmp {
namespace dependencies_graph {

template<class State>
MarkovKernelNodeBase<State>::MarkovKernelNodeBase(
  const std::vector<int>& dependentVariableIds)
  : dependentVariableIds(dependentVariableIds) {
};

template<class State, class Lambda>
MarkovKernelNode<State, Lambda>::MarkovKernelNode(
  const std::vector<int>& dependentVariableIds,
  const Lambda& lambda)
  : MarkovKernelNode(dependentVariableIds, lambda, dependentVariableIds) {
}

template<class State, class Lambda>
MarkovKernelNode<State, Lambda>::MarkovKernelNode(
  const std::vector<int>& dependentVariableIds,
  const Lambda& lambda,
  const std::vector<int>& requiredVariableIdsForAccess)
  : MarkovKernelNodeBase<State>(dependentVariableIds),
    lambda_(lambda),
    requiredVariableIdsForAccess_(requiredVariableIdsForAccess) {
}

template<class State, class Lambda>
State MarkovKernelNode<State, Lambda>::jump(const State& state) {
  auto stateSubvector = state.getSubvector(this->requiredVariableIdsForAccess_);
  auto modifiedSubvector = this->lambda_(stateSubvector);
  return state.constructStateWithModifiedVariables(
    this->requiredVariableIdsForAccess_, modifiedSubvector);
}

}
}
