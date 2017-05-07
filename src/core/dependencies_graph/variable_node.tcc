#pragma once

namespace pdmp {
namespace dependencies_graph {

VariableNode::VariableNode(const std::vector<int>& dependentFactorIds)
  : dependentFactorIds(dependentFactorIds) {
}

}
}
