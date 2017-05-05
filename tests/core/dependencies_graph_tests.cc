#include <algorithm>
#include <functional>

#include <gtest/gtest.h>

#include "core/dependencies_graph/dependencies_graph.h"

struct DummyMarkovKernelNode {
  std::vector<int> dependentVariableIds;
};

struct DummyVariableNode {
  std::vector<int> dependentFactorIds;
};

struct DummyFactorNode {
};

// Flow with identity as dependencies.
class DummyFlow1 {
 public:
  static std::vector<int> getDependentVariableIds(int id, int dim) {
    return {id};
  }
};

// Flow with identity and adjacent variable as dependencies.
class DummyFlow2 {
 public:
  static std::vector<int> getDependentVariableIds(int id, int dim) {
    return {id, (id + 1) % dim};
  }
};

const int kNumberOfFactors = 3;
const int kStateSpaceDimension = 4;
using DependenciesGraph = pdmp::dependencies_graph::DependenciesGraph<
  kNumberOfFactors,
  kStateSpaceDimension,
  DummyMarkovKernelNode,
  DummyVariableNode,
  DummyFactorNode>;

// A text fixture class for dependencies graph testing.
// Kernel0 --modifies--> Variable1
// Kernel1 --modifies--> Variable0, Variable3
// Kernel2 --modifies--> Variable2
// Factor0 needs Variable0 and Variable1
// Factor1 needs Variable1
// Factor2 needs Variable1 and Variable3
class DependenciesGraphTests : public ::testing::Test {

 protected:

  DependenciesGraphTests()
    : graph_(
      {DummyMarkovKernelNode{{1}}, // Kernel0
       DummyMarkovKernelNode{{0, 3}}, // Kernel1
       DummyMarkovKernelNode{{2}}}, // Kernel2
      {DummyVariableNode{{0}}, // Variable0
       DummyVariableNode{{0, 1, 2}}, // Variable1
       DummyVariableNode{{}}, // Variable2
       DummyVariableNode{{2}}}, // Variable3
      {DummyFactorNode(), DummyFactorNode(), DummyFactorNode()}) {
  }

  DependenciesGraph graph_;
};

namespace {

// We will only compare contents of the vectors, and not the order.
// This is a helper for sorting.
std::vector<int> sorted(std::vector<int> vector) {
  std::sort(vector.begin(), vector.end(), std::less<int>());
  return vector;
}

}

TEST_F(DependenciesGraphTests, TestDependenciesAreCalculatedCorrectly) {
  std::vector<int> expectedDependencies{0, 1, 2};
  for (int i = 0; i < 2; i++) {
    auto dependencies = graph_.getFactorDependencies<DummyFlow1>(0);
    EXPECT_TRUE(sorted(dependencies) == expectedDependencies);
  }
}

TEST_F(DependenciesGraphTests, TestDependenciesAreCalculatedCorrectly2) {
  std::vector<int> expectedDependencies{0, 2};
  for (int i = 0; i < 2; i++) {
    auto dependencies = graph_.getFactorDependencies<DummyFlow1>(1);
    EXPECT_TRUE(sorted(dependencies) == expectedDependencies);
  }
}

TEST_F(DependenciesGraphTests, TestDependenciesAreCalculatedCorrectly3) {
  std::vector<int> expectedDependencies{};
  for (int i = 0; i < 2; i++) {
    auto dependencies = graph_.getFactorDependencies<DummyFlow1>(2);
    EXPECT_TRUE(sorted(dependencies) == expectedDependencies);
  }
}

TEST_F(DependenciesGraphTests, TestDependenciesAreCalculatedCorrectly4) {
  std::vector<int> expectedDependencies{0, 1, 2};
  for (int i = 0; i < 2; i++) {
    auto dependencies = graph_.getFactorDependencies<DummyFlow2>(0);
    EXPECT_TRUE(sorted(dependencies) == expectedDependencies);
  }
}

TEST_F(DependenciesGraphTests, TestDependenciesAreCalculatedCorrectly5) {
  std::vector<int> expectedDependencies{0, 1, 2};
  for (int i = 0; i < 2; i++) {
    auto dependencies = graph_.getFactorDependencies<DummyFlow2>(1);
    EXPECT_TRUE(sorted(dependencies) == expectedDependencies);
  }
}

TEST_F(DependenciesGraphTests, TestDependenciesAreCalculatedCorrectly6) {
  std::vector<int> expectedDependencies{2};
  for (int i = 0; i < 2; i++) {
    auto dependencies = graph_.getFactorDependencies<DummyFlow2>(2);
    EXPECT_TRUE(sorted(dependencies) == expectedDependencies);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

