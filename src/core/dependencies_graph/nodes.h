#pragma once

#include <vector>

namespace pdmp {
namespace dependencies_grap {

/**
 * In this file we declare three types, serving a particular implementation
 * that can be used with the dependencies graph.
 */


/**
 * A class for representing nodes associated with Markov kernels.
 */
template<class Lambda>
class MarkovKernelNode {

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
  template<class State>
  State jump(const State& state);

  std::vector<int> dependentVariableIds;

 private:

  Lambda lambda_;
};

/**
 * A struct for representing nodes associated to variables.
 */
struct VariableNode {

  /**
   * @dependentFactorIds
   *   The ids of intensity factors, which depend on this variable.
   */
  VariableNode(const std::vector<int>& dependenFactorIds);

  const std::vector<int> dependentFactorIds;
};

/**
 * A class for representing nodes associated to intensity factors.
 */
template<class Lambda>
class FactorNode {

 public:

  FactorNode(
    const std::vector<int>& dependentVariableIds, const Lambda& lambda);

  /**
   *  Evaluates the indensity at this given state.
   */
  template<class State>
  auto evaluateIntensity(const State& state);

  std::vector<int> dependentVariableIds;

 private:

  Lambda lambda_;
};

}
}

#include "nodes.tcc"
