#pragma once

#include <memory>

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
  State jump(const State& state, const HostClass& hostClass);

 private:

  const MarkovKernels& markovKernels_;

};

}
}

#include "markov_kernel.tcc"
