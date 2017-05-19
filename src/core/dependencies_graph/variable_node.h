#pragma once

#include <vector>

namespace pdmp {
namespace dependencies_graph {

/**
 * A struct for representing nodes associated to variables.
 */
struct VariableNode {


  VariableNode() = default;

  /**
   * @dependentFactorIds
   *   The ids of intensity factors, which depend on this variable.
   */
  VariableNode(const std::vector<int>& dependenFactorIds);

  std::vector<int> dependentFactorIds;
};

}
}

#include "variable_node.tcc"
