#pragma once

namespace pdmp {
namespace dependencies_grap {

template<class Lambda>
MarkovKernelNode<Lambda>::MarkovKernelNode(
  const std::vector<int>& dependentVariableIds,
  const Lambda& lambda)
  : dependentVariableIds(dependentVariableIds),
    lambda_(lambda) {
}

template<class Lambda>
template<class State>
State MarkovKernelNode<Lambda>::jump(const State& state) {
  auto stateSubvector = state.getSubvector(this->dependentVariableIds);
  auto modifiedSubvector = this->lambda_(stateSubvector);
  return state.constructStateWithModifiedVariables(
    this->dependentVariableIds, modifiedSubvector);
}

VariableNode::VariableNode(const std::vector<int>& dependentFactorIds)
  : dependentFactorIds(dependentFactorIds) {
}

template<class Lambda>
FactorNode<Lambda>::FactorNode(
  const std::vector<int>& dependentVariableIds,
  const Lambda& lambda)
  : dependentVariableIds(dependentVariableIds),
    lambda_(lambda) {
}

template<class Lambda>
template<class State>
auto FactorNode<Lambda>::evaluateIntensity(const State& state) {
  auto stateSubvector = state.getSubvector(this->dependentVariableIds);
  return this->lambda_(stateSubvector);
}

}
}
