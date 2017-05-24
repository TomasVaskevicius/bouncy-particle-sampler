#pragma once

#include <memory>
#include <vector>

namespace pdmp {
namespace dependencies_graph {

/**
 * Markov kernel policy implementation based on the dependencies graph.
 */
template<class DependenciesGraph>
class MarkovKernel {

 public:

  using MarkovKernels = typename DependenciesGraph::MarkovKernelNodes;

  MarkovKernel(std::shared_ptr<DependenciesGraph> dependenciesGraph);

  /**
   * Checks which Poisson process factor was the last one to be counted
   * and invokes appropriate Markov kernel.
   */
  template<class State, class HostClass>
  State jump(State&& state, const HostClass& hostClass);

  /**
   * Returns ids of variables that were modified by the last jump.
   */
  std::vector<int> getLastModifiedVariables() const;

 private:

  const MarkovKernels& markovKernels_;
  int lastFactorId_;
};

}
}

#include "markov_kernel.tcc"
