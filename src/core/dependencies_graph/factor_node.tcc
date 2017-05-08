#pragma once

namespace pdmp {
namespace dependencies_graph {

template<class State>
FactorNodeBase<State>::FactorNodeBase(
  const std::vector<int>& dependentVariableIds)
  : dependentVariableIds(dependentVariableIds) {
}

template<class State, class PoissonProcessLambda, class IntensityLambda>
FactorNode<State, PoissonProcessLambda, IntensityLambda>::FactorNode(
  const std::vector<int>& dependentVariableIds,
  const PoissonProcessLambda& poissonProcessLambda,
  const IntensityLambda& intensityLambda)
  : FactorNodeBase<State>(dependentVariableIds),
    poissonProcessLambda_(poissonProcessLambda),
    intensityLambda_(intensityLambda) {
}

template<class State, class PoissonProcessLambda, class IntensityLambda>
typename FactorNode<State, PoissonProcessLambda, IntensityLambda>::RealType
FactorNode<State, PoissonProcessLambda, IntensityLambda>
  ::evaluateIntensity(const State& state) {

  return this->intensityLambda_(state, *this);
}

template<class State, class PoissonProcessLambda, class IntensityLambda>
PoissonProcessResultPtr FactorNode<State, PoissonProcessLambda, IntensityLambda>
  ::getPoissonProcessResult(const State& state) {

  return this->poissonProcessLambda_(state, *this);
}

}
}
