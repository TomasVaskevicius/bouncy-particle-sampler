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
  State&& state, const HostClass& hostClass) {

  this->lastFactorId_ = hostClass.getLastFactorId();
  return markovKernels_[this->lastFactorId_]->jump(std::forward<State>(state));
}

template<class DependenciesGraph>
std::vector<int> MarkovKernel<DependenciesGraph>
  ::getLastModifiedVariables() const {

  return markovKernels_[this->lastFactorId_]->dependentVariableIds;
}

}
}
