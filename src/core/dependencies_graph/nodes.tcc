#pragma once

#include <stdexcept>

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

template<class PoissonProcessLambda, class IntensityLambda>
FactorNode<PoissonProcessLambda, IntensityLambda>::FactorNode(
  const std::vector<int>& dependentVariableIds,
  const PoissonProcessLambda& poissonProcessLambda,
  const IntensityLambda& intensityLambda)
  : dependentVariableIds(dependentVariableIds),
    poissonProcessLambda_(poissonProcessLambda),
    intensityLambda_(intensityLambda) {
}

template<class Input>
auto NoOpIntensity::operator()(Input) {
  throw std::runtime_error("Called unimplemented intensity method.");
}

template<class PoissonProcessLambda, class IntensityLambda>
template<class State>
auto FactorNode<PoissonProcessLambda, IntensityLambda>
  ::evaluateIntensity(const State& state) {

  auto stateSubvector = state.getSubvector(this->dependentVariableIds);
  return this->intensityLambda_(stateSubvector);
}

template<class PoissonProcessLambda, class IntensityLambda>
template<class State>
auto FactorNode<PoissonProcessLambda, IntensityLambda>
  ::getPoissonProcessResult(const State& state) {

  auto stateSubvector = state.getSubvector(this->dependentVariableIds);
  return this->poissonProcessLambda_(stateSubvector);
}

}
}
