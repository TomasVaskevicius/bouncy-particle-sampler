#pragma once

#include <memory>
#include <set>
#include <vector>

#include "core/policies/linear_flow.h"

namespace pdmp {
namespace dependencies_graph {

/**
 * A directed graph, representing dependencies from Markov kernels to variables
 * to intensity factors.
 *
 * The number of Markov kernels and intensity factors should be the same and is
 * denoted by template parameter N.
 * The number of variable nodes must be equal to the state space dimensionality.
 */
template<class MarkovKernelNode_t, class VariableNode_t, class FactorNode_t>
class DependenciesGraph {

 public:

  using MarkovKernelNodes = std::vector<std::shared_ptr<MarkovKernelNode_t>>;
  using VariableNodes = std::vector<std::shared_ptr<VariableNode_t>>;
  using FactorNodes = std::vector<std::shared_ptr<FactorNode_t>>;

  DependenciesGraph(
    const MarkovKernelNodes& markovKernelNodes,
    const VariableNodes& variableNodes,
    const FactorNodes& factorNodes);

  /**
   * Returns ids of factors, dependent on a given factor.
   *
   * The dependencies are calculated as follows:
   * 1) Calculate all variables, dependent on the associated markov kernel
   *    (i.e. a Markov kernel, which has the same id as a given factor).
   * 2) Calculate the variable dependencies based on the Flow template.
   *    The flow template needs to provide a method
   *    std::vector<int> getDependentVariables(int variableId, int dim).
   * 3) Find all the factors, dependent on the variables calculated above.
   *
   * @param factorId
   *   The factor, for which we want to find the dependent factors.
   * @return
   *   A vector of dependent factor ids.
   */
  template<class Flow = LinearFlow>
  const std::vector<int>& getFactorDependencies(int factorId);

  const MarkovKernelNodes markovKernelNodes;
  const VariableNodes variableNodes;
  const FactorNodes factorNodes;

 private:

  // A bool array, specifying if factor dependencies for i-th factor
  // were already cached.
  std::vector<bool> areFactorDependenciesCached_;

  // Once we calculate dependencies for a given factor, we cache the
  // results here.
  std::vector<std::vector<int>> factorDependenciesCache_;

};

}
}

#include "dependencies_graph.tcc"
