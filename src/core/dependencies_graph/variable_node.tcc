#pragma once

namespace pdmp {
namespace dependencies_grap {

VariableNode::VariableNode(const std::vector<int>& dependentFactorIds)
  : dependentFactorIds(dependentFactorIds) {
}

}
}
