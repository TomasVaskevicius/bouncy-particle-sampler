#include <memory>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Eigen/Core>

#include "core/dependencies_graph/dependencies_graph.h"
#include "core/dependencies_graph/factor_node.h"
#include "core/policies/linear_flow.h"
#include "core/policies/poisson_process.h"
#include "core/state_space/position_and_velocity_state.h"

#include "dummy_nodes.h"

using namespace testing;
using namespace pdmp;
using namespace pdmp::dependencies_graph;
using namespace std;

/**
 * Tests for Poisson process simulation policy with mock FactorNodes
 * but real DependenciesGraph.
 */

const int kDim = 4;
const int kNumberOfFactors = 3;
using RealType = float;
using State = PositionAndVelocityState<RealType, kDim>;
using RealVector = typename State::RealVector<kDim/2>;
using PoissonProcessResultPtr = std::shared_ptr<PoissonProcessResultBase>;

struct MockFactorNode : FactorNodeBase<State> {
  MockFactorNode(const std::vector<int>& dummyVec)
    : FactorNodeBase<State>(dummyVec) {}
  MOCK_METHOD1(evaluateIntensity, RealType(const State&));
  MOCK_METHOD1(getPoissonProcessResult, PoissonProcessResultPtr(const State&));
};

using MyDependenciesGraph = DependenciesGraph<
  DummyMarkovKernelNode,
  DummyVariableNode,
  MockFactorNode>;

/**
 * We start with a simple graph. Associated Markov kernels act on the same
 * variables that their intensity factors depend on.
 * Factor graph visualization:
 *
 * var0 ----- factor0
 *
 * var1
 *
 * var2 ----- factor1
 *         -
 *        -
 *       -
 *      -
 * var3 ----- factor2
 *
 * Factor dependencies:
 * 0 ---> 0
 * 1 ---> 0, 1, 2
 * 3 ---> 1, 2
 */
class PoissonProcessSimulationTests : public ::testing::Test {

 protected:

  PoissonProcessSimulationTests()
    : graph_(std::make_shared<MyDependenciesGraph>(
      std::vector<shared_ptr<DummyMarkovKernelNode>>{
        make_shared<DummyMarkovKernelNode>(std::vector<int>{0}), // Kernel0
        make_shared<DummyMarkovKernelNode>(std::vector<int>{2, 3}), // Kernel1
        make_shared<DummyMarkovKernelNode>(std::vector<int>{3}) // Kernel2
      },
      std::vector<shared_ptr<DummyVariableNode>>{
        make_shared<DummyVariableNode>(std::vector<int>{0}), // Variable0
        make_shared<DummyVariableNode>(std::vector<int>{}), // Variable1
        make_shared<DummyVariableNode>(std::vector<int>{1}), // Variable2
        make_shared<DummyVariableNode>(std::vector<int>{1, 2}) // Variable3
      },
      std::vector<shared_ptr<MockFactorNode>>{
        make_shared<MockFactorNode>(std::vector<int>{}), // Factor0
        make_shared<MockFactorNode>(std::vector<int>{}), // Factor1
        make_shared<MockFactorNode>(std::vector<int>{}) // Factor2
      })),
      initialState_(RealVector(1.0f, 2.0f), RealVector(1.0f, 2.0f)),
      poissonProcess_(this->graph_) {
  }

  void setUpReturnObjectForMockFactorNode(
    int nodeId,
    const State& state,
    shared_ptr<PoissonProcessResultBase>* result) {

    EXPECT_CALL(
      *(this->graph_->factorNodes[nodeId]), getPoissonProcessResult(state))
        .Times(1)
        .WillOnce(Return(*result));
  }

  std::shared_ptr<MyDependenciesGraph> graph_;
  const State initialState_;
  PoissonProcess<MyDependenciesGraph> poissonProcess_;
};


bool areEqual(const float& f1, const float& f2) {
  return std::abs(f1 - f2) < std::numeric_limits<float>::min();
}

TEST_F(
  PoissonProcessSimulationTests,
  TestFirstIterationSimulatesTimeForEachFactorAndReturnsCorrectTime) {

  shared_ptr<PoissonProcessResultBase> result
    = make_shared<PoissonProcessResult<>>(1.0f);
  for (int i = 0; i < kNumberOfFactors; i++) {
    setUpReturnObjectForMockFactorNode(i, initialState_, &result);
  }
  poissonProcess_.getJumpTime(initialState_, LinearFlow());
}

TEST_F(
  PoissonProcessSimulationTests,
  TestCorrectTimeReturnedInFirstIteratrion) {

  shared_ptr<PoissonProcessResultBase> result0
    = make_shared<PoissonProcessResult<>>(1.0f);
  shared_ptr<PoissonProcessResultBase> result1
    = make_shared<PoissonProcessResult<>>(2.0f);
  shared_ptr<PoissonProcessResultBase> result2
    = make_shared<PoissonProcessResult<>>(3.0f);

  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);
  setUpReturnObjectForMockFactorNode(2, initialState_, &result2);

  auto jumpTime = poissonProcess_.getJumpTime(initialState_, LinearFlow());
  EXPECT_TRUE(areEqual(jumpTime, 1.0f));
}

TEST_F(
  PoissonProcessSimulationTests,
  TestCorrectFactorsAreResampledInSecondIteration) {

  shared_ptr<PoissonProcessResultBase> result0
    = make_shared<PoissonProcessResult<>>(1.5f);
  shared_ptr<PoissonProcessResultBase> result1
    = make_shared<PoissonProcessResult<>>(2.0f);
  shared_ptr<PoissonProcessResultBase> result2
    = make_shared<PoissonProcessResult<>>(3.0f);

  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);
  setUpReturnObjectForMockFactorNode(2, initialState_, &result2);

  poissonProcess_.getJumpTime(initialState_, LinearFlow());

  // Factor0 should have returned, hence only the first factor
  // needs to be resampled.
  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);

  auto jumpTime = poissonProcess_.getJumpTime(initialState_, LinearFlow());
  EXPECT_TRUE(areEqual(jumpTime, 0.5f));
}

TEST_F(
  PoissonProcessSimulationTests,
  TestCorrectFactorsAreResampledInSecondIteration2) {

  shared_ptr<PoissonProcessResultBase> result0
    = make_shared<PoissonProcessResult<>>(2.0f);
  shared_ptr<PoissonProcessResultBase> result1
    = make_shared<PoissonProcessResult<>>(1.5f);
  shared_ptr<PoissonProcessResultBase> result2
    = make_shared<PoissonProcessResult<>>(3.0f);

  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);
  setUpReturnObjectForMockFactorNode(2, initialState_, &result2);

  poissonProcess_.getJumpTime(initialState_, LinearFlow());

  // Factor1 should have returned, hence all factors need to be resampled.
  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);
  setUpReturnObjectForMockFactorNode(2, initialState_, &result2);

  auto jumpTime = poissonProcess_.getJumpTime(initialState_, LinearFlow());
  EXPECT_TRUE(areEqual(jumpTime, 1.5f));
}

TEST_F(
  PoissonProcessSimulationTests,
  TestCorrectFactorsAreResampledInSecondIteration3) {

  shared_ptr<PoissonProcessResultBase> result0
    = make_shared<PoissonProcessResult<>>(2.0f);
  shared_ptr<PoissonProcessResultBase> result1
    = make_shared<PoissonProcessResult<>>(1.75f);
  shared_ptr<PoissonProcessResultBase> result2
    = make_shared<PoissonProcessResult<>>(1.5f);

  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);
  setUpReturnObjectForMockFactorNode(2, initialState_, &result2);

  poissonProcess_.getJumpTime(initialState_, LinearFlow());

  // Factor2 should have returned, hence all factors 1 and 2 need
  // to be resampled.
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);
  setUpReturnObjectForMockFactorNode(2, initialState_, &result2);

  float returned = poissonProcess_.getJumpTime(initialState_, LinearFlow());
  EXPECT_TRUE(areEqual(returned, 0.5f));
}

TEST_F(PoissonProcessSimulationTests, TestThinningProcedureWorks) {

  auto reject = [] () { return false; };
  shared_ptr<PoissonProcessResultBase> result0
    = make_shared<PoissonProcessResult<>>(1.75f);
  shared_ptr<PoissonProcessResultBase> result1
    = make_shared<PoissonProcessResult<>>(2.0f);
  shared_ptr<PoissonProcessResultBase> result2
    = make_shared<PoissonProcessResult<decltype(reject)>>(1.5f, reject);

  setUpReturnObjectForMockFactorNode(0, initialState_, &result0);
  setUpReturnObjectForMockFactorNode(1, initialState_, &result1);

  // Factor2 will be thinned out, so getPoissonProcessResult will be called
  // two times on it.
  EXPECT_CALL(
    *(this->graph_->factorNodes[2]), getPoissonProcessResult(_))
        .Times(2)
        .WillRepeatedly(Return(result2));

  float returned = poissonProcess_.getJumpTime(initialState_, LinearFlow());
  EXPECT_TRUE(areEqual(returned, 1.75f));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

