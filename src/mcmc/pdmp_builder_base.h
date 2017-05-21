#pragma once

#include "core/pdmp.h"
#include "core/dependencies_graph/dependencies_graph.h"
#include "core/dependencies_graph/factor_node.h"
#include "core/dependencies_graph/markov_kernel_node.h"
#include "core/dependencies_graph/variable_node.h"
#include "core/policies/linear_flow.h"
#include "core/policies/markov_kernel.h"
#include "core/policies/poisson_process.h"

namespace pdmp {
namespace mcmc {


/**
 * A helper class for building dependencies graph based PDMPs for MCMC
 * algorithms.
 */
template<class State, class Flow>
class PdmpBuilderBase {

 public:

  using FactorNodeBase = dependencies_graph::FactorNodeBase<State>;
  using MarkovKernelNodeBase = dependencies_graph::MarkovKernelNodeBase<State>;
  using VariableNode = dependencies_graph::VariableNode;
  using DependenciesGraph = dependencies_graph::DependenciesGraph<
    MarkovKernelNodeBase, VariableNode, FactorNodeBase>;

  PdmpBuilderBase(int stateSpaceDimension);

  /**
   * Adds a factor node to the graph. Note that care must be taken by the
   * called to add the factor and Markov kernel nodes in the correct order.
   *
   * @param dependentVariableIds
   *   The variables needed by this Poisson process simulation factor.
   *   All callable objects will be provided with a subvector of a real state,
   *   where the subvector will be extracted using these ids.
   * @param poissonProcessStrategy
   *   A callable object, that given a subvector of state returns a tuple of
   *   jump time proposal and a thinning functor.
   * @param intensity
   *   A callable object, evaluating intensity at a given state subvector.
   */
  template<
    class F1,
    class F2 = decltype(pdmp::dependencies_graph::noOpIntensity)>
  void addFactorNode(
    const std::vector<int>& dependentVariableIds,
    F1 poissonProcessStrategy,
    F2 intensity = pdmp::dependencies_graph::noOpIntensity);

  /**
   * Adds a Markov kernel node to the graph. Note that care must be taken by the
   * called to add the factor and Markov kernel nodes in the correct order.
   *
   * @param variableIdsNeededByKernel
   *   The variables that this Markov kernel will need access to.
   * @param variableIdsModifiableByKernel
   *   The variables that will be modified by this kernel. Needs to be a
   *   subset of the variableIdsNeededByKernel.
   * @param kernel
   *   A callable object, taking the state subvector (with ids given by
   *   variableIdsNeededByKernel) and returning a modified vector,
   *   where only the variableIdsModifiableByKernel are changed.
   */
  template<class F>
  void addMarkovKernelNode(
    const std::vector<int>& variableIdsNeededByKernel,
    const std::vector<int>& variableIdsModifiableByKernel,
    F kernel);


  /**
   * Returns a PDMP based on the dependencies graph created.
   */
  auto build();

 protected:

  int numberOfFactorsAdded_{0};
  std::vector<std::shared_ptr<VariableNode>> variableNodes_;
  std::vector<std::shared_ptr<FactorNodeBase>> factorNodes_;
  std::vector<std::shared_ptr<MarkovKernelNodeBase>> markovKernelNodes_;
};

}
}

#include "pdmp_builder_base.tcc"
