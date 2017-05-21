#pragma once

#include <stdexcept>

namespace pdmp {
namespace mcmc {

template<class State, class Flow>
PdmpBuilderBase<State, Flow>::PdmpBuilderBase(
  int stateSpaceDimension) {

  for (int i = 0; i < stateSpaceDimension; i++) {
    variableNodes_.push_back(std::make_shared<VariableNode>());
  }
}

template<class State, class Flow>
template<class F1, class F2>
void PdmpBuilderBase<State, Flow>::addFactorNode(
  const std::vector<int>& dependentVariableIds,
  F1 poissonProcessStrategy,
  F2 intensity) {

  std::shared_ptr<FactorNodeBase> factorNode = std::make_shared<
    dependencies_graph::FactorNode<
      State, decltype(poissonProcessStrategy), Flow, decltype(intensity)>>(
        dependentVariableIds, poissonProcessStrategy, intensity);
  factorNodes_.push_back(factorNode);

  // Update the variable nodes, on which the current factor depends.
  for (int i = 0; i < dependentVariableIds.size(); i++) {
    variableNodes_.at(dependentVariableIds[i])->dependentFactorIds.push_back(
      numberOfFactorsAdded_);
  }
  numberOfFactorsAdded_++;
}

template<class State, class Flow>
template<class F>
void PdmpBuilderBase<State, Flow>::addMarkovKernelNode(
  const std::vector<int>& variableIdsNeededByKernel,
  const std::vector<int>& variableIdsModifiableByKernel,
  F kernel) {

  std::shared_ptr<MarkovKernelNodeBase> markovKernelNode = std::make_shared<
    dependencies_graph::MarkovKernelNode<State, decltype(kernel)>>(
      variableIdsModifiableByKernel,
      kernel,
      variableIdsNeededByKernel);
  markovKernelNodes_.push_back(markovKernelNode);
}

template<class State, class Flow>
auto PdmpBuilderBase<State, Flow>::build() {
  auto dependenciesGraph = std::make_shared<DependenciesGraph>(
    markovKernelNodes_, variableNodes_, factorNodes_);
  auto args = std::make_tuple(dependenciesGraph);
  return Pdmp<
    dependencies_graph::PoissonProcess<DependenciesGraph>,
    dependencies_graph::MarkovKernel<DependenciesGraph>,
    Flow>(args, args);
}

}
}
