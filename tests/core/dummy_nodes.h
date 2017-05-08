#pragma once

#include <vector>

struct DummyMarkovKernelNode {
  DummyMarkovKernelNode(const std::vector<int>& dependentVariableIds)
    : dependentVariableIds(dependentVariableIds) {}
  std::vector<int> dependentVariableIds;
};

struct DummyVariableNode {
  DummyVariableNode(const std::vector<int>& dependentFactorIds)
    : dependentFactorIds(dependentFactorIds) {}
  std::vector<int> dependentFactorIds;
};

struct DummyFactorNode {
};
