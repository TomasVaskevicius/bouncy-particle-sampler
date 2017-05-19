#pragma once

namespace pdmp {
namespace dependencies_graph {

template<class State>
FactorNodeBase<State>::FactorNodeBase(
  const std::vector<int>& dependentVariableIds)
  : dependentVariableIds(dependentVariableIds) {
}

template<
  class State, class PoissonProcessLambda, class Flow, class IntensityLambda>
FactorNode<State, PoissonProcessLambda, Flow, IntensityLambda>::FactorNode(
  const std::vector<int>& dependentVariableIds,
  const PoissonProcessLambda& poissonProcessLambda,
  const IntensityLambda& intensityLambda)
  : FactorNodeBase<State>(dependentVariableIds),
    poissonProcessLambda_(poissonProcessLambda),
    intensityLambda_(intensityLambda) {
}

template<
  class State, class PoissonProcessLambda, class Flow,  class IntensityLambda>
typename FactorNode<State, PoissonProcessLambda, Flow, IntensityLambda>
::RealType FactorNode<State, PoissonProcessLambda, Flow, IntensityLambda>
  ::evaluateIntensity(const State& state, const RealType& time) {

  auto advancedState = Flow::advanceStateByFlow(state, time);
  auto stateSubvector = advancedState.getSubvector(this->dependentVariableIds);
  return this->intensityLambda_(stateSubvector, *this);
}

template<
  class State, class PoissonProcessLambda, class Flow, class IntensityLambda>
PoissonProcessResultPtr
FactorNode<State, PoissonProcessLambda, Flow, IntensityLambda>
  ::getPoissonProcessResult(const State& state) {

  auto stateSubvector = state.getSubvector(this->dependentVariableIds);
  return this->poissonProcessLambda_(stateSubvector, *this, state);
}

}
}
