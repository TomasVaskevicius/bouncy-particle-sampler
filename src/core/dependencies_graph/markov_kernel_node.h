#pragma once

#include <vector>

namespace pdmp {
namespace dependencies_graph {

template<class State>
struct MarkovKernelNodeBase {
  MarkovKernelNodeBase(const std::vector<int>& dependentVariableIds);
  ~MarkovKernelNodeBase() = default;
  virtual State jump(const State& state) = 0;
  const std::vector<int> dependentVariableIds;
};

/**
 * A class for representing nodes associated with Markov kernels.
 */
template<class State, class Lambda>
class MarkovKernelNode : public MarkovKernelNodeBase<State> {

 public:

  /**
   * This constructor assumes, that this Markov kernel will only need
   * access to the variables that are going to be modified.
   *
   * @lambda
   *   A lambda closure object, which will be called by the jump method.
   * @dependentVariableIds
   *   The ids of the variables, which will be modified by
   *   the jump method.
   */
  MarkovKernelNode(
    const std::vector<int>& dependentVariableIds,
    const Lambda& lambda);

  /**
   * @lambda
   *   A lambda closure object, which will be called by the jump method.
   *   Given a state vector, should return a state vector of the same size.
   *   Only variables with ids dependentVariableIds can be modified.
   * @dependentVariableIds
   *   The ids of the variables, which will be modified by
   *   the jump method.
   * @requiredVariableIdsForAccess
   *   The variables with these ids will be passed to the jump kernel lambda.
   *   This vector should be a superset of ids given in dependentVariableIds.
   */
  MarkovKernelNode(
    const std::vector<int>& dependentVariableIds,
    const Lambda& lambda,
    const std::vector<int>& requiredVariableIdsForAccess);

  /**
   * Applieds the Markov kernel jump on a given state.
   */
  virtual State jump(const State& state) override final;

 private:

  Lambda lambda_;
  std::vector<int> requiredVariableIdsForAccess_;
};

}
}

#include "markov_kernel_node.tcc"
