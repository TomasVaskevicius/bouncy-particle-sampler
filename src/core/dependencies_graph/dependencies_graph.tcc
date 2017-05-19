#pragma once

#include <iterator>
#include <stdexcept>

namespace {

constexpr void checkFactorIdBounds(int factorId, int maxId) {
  (factorId < 0 || factorId >= maxId) ?
    throw std::out_of_range("The given factor id " + std::to_string(factorId) +
                            " is out of bounds. Should be between 0 and " +
                            std::to_string(maxId - 1) + ".")
    : 0;
}

// A helper method for caching factor dependencies.
template<class Flow, class Graph>
std::vector<int> computeFactorDependencies(int factorId, const Graph& graph) {
  std::set<int> variableDependencies;

  // First add all variable ids, that are modified by the current markov kernel.
  auto markovKernelNode = graph.markovKernelNodes[factorId];
  for (const int& id : markovKernelNode->dependentVariableIds) {
    variableDependencies.insert(id);
  }

  // Now expand the dependencies set, based on the Flow policy used.
  const int stateSpaceDim = graph.variableNodes.size();
  std::set<int> variableToVariableDependencies;
  for (const int& depVar : variableDependencies) {
    for (const int& id : Flow::getDependentVariableIds(depVar, stateSpaceDim)) {
      variableToVariableDependencies.insert(id);
    }
  }

  // Now find factors, dependent on variables in variableDependencies set.
  std::set<int> factorDependencies;
  for (const int& id : variableToVariableDependencies) {
    for (const int& depFactorId : graph.variableNodes[id]->dependentFactorIds) {
      factorDependencies.insert(depFactorId);
    }
  }

  // Convert the set to a vector and return.
  std::vector<int> dependenciesVector(
    std::begin(factorDependencies), std::end(factorDependencies));
  return dependenciesVector;
}

}

namespace pdmp {
namespace dependencies_graph {

template<class MarkovKernelNode_t, class VariableNode_t, class FactorNode_t>
DependenciesGraph<MarkovKernelNode_t, VariableNode_t, FactorNode_t>
  ::DependenciesGraph(
    const MarkovKernelNodes& markovKernelNodes,
    const VariableNodes& variableNodes,
    const FactorNodes& factorNodes)
  : markovKernelNodes(markovKernelNodes),
    variableNodes(variableNodes),
    factorNodes(factorNodes),
    areFactorDependenciesCached_(factorNodes.size(), false),
    factorDependenciesCache_(factorNodes.size(), std::vector<int>()) {

  if (factorNodes.size() != markovKernelNodes.size()) {
    throw std::runtime_error(
      "Trying to create a dependencies graph with different number of factor "
      "and Markov kernel nodes.");
  }
}

template<class MarkovKernelNode_t, class VariableNode_t, class FactorNode_t>
template<class Flow>
const std::vector<int>&
DependenciesGraph<MarkovKernelNode_t, VariableNode_t, FactorNode_t>
  ::getFactorDependencies(int factorId) {

  checkFactorIdBounds(factorId, factorNodes.size());
  if (!areFactorDependenciesCached_[factorId]) {
    factorDependenciesCache_[factorId] =
      computeFactorDependencies<Flow>(factorId, *this);
    areFactorDependenciesCached_[factorId] = true;
  }
  return factorDependenciesCache_[factorId];
}

}
}

