#pragma once

#include <vector>

namespace pdmp {
namespace dependencies_grap {

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
class MarkovKernelNode : MarkovKernelNodeBase<State> {

 public:

  /**
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
   * Applieds the Markov kernel jump on a given state.
   */
  virtual State jump(const State& state) final;

 private:

  Lambda lambda_;
};

}
}

#include "markov_kernel_node.tcc"
