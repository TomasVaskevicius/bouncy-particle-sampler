#pragma once

namespace pdmp {
namespace dependencies_graph {

template<class DependenciesGraph>
MarkovKernel<DependenciesGraph>::MarkovKernel
  (std::shared_ptr<DependenciesGraph> dependenciesGraph)
  : markovKernels_(dependenciesGraph->markovKernelNodes) {
}


template<class DependenciesGraph>
template<class State, class HostClass>
State MarkovKernel<DependenciesGraph>::jump(
  const State& state, const HostClass& hostClass) {

  int factorId = hostClass.getLastFactorId();
  return markovKernels_[factorId]->jump(state);
}

}
}
