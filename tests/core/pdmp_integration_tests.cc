#include <array>
#include <cmath>
#include <memory>
#include <tuple>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/pdmp.h"
#include "core/dependencies_graph/dependencies_graph.h"
#include "core/dependencies_graph/factor_node.h"
#include "core/dependencies_graph/markov_kernel_node.h"
#include "core/dependencies_graph/variable_node.h"
#include "core/policies/linear_flow.h"
#include "core/policies/markov_kernel.h"
#include "core/policies/poisson_process.h"
#include "core/state_space/position_and_velocity_state.h"

/**
 * Integration tests for piecewise-deterministic Markov processes.
 * Only the random number generators are mocked in these tests.
 */

using namespace pdmp;
using namespace pdmp::dependencies_graph;
using namespace std;
using namespace testing;

const int kStateSpaceDim = 4;
using RealType = float;
using State = PositionAndVelocityState<RealType, kStateSpaceDim>;
using PtrToMarkovKernelNode = shared_ptr<MarkovKernelNodeBase<State>>;
using PtrToFactorNode = shared_ptr<FactorNodeBase<State>>;
using PtrToVariableNode = shared_ptr<VariableNode>;
using MyDependenciesGraph = DependenciesGraph<
  MarkovKernelNodeBase<State>,
  VariableNode,
  FactorNodeBase<State>>;
using MyPdmp = Pdmp<
  PoissonProcess<MyDependenciesGraph>,
  MarkovKernel<MyDependenciesGraph>>;

struct MockRNG {
  MOCK_METHOD0(getUnif01RandomVariable, float());
};

namespace {

std::vector<PtrToMarkovKernelNode> setUpMarkovKernelNodes(
  MockRNG& mockRng0, MockRNG& mockRng1) {

  // Kernel0 multiplied associated variables by 2.
  auto kernel0 = [] (typename State::DynamicRealVector vector) {
    vector *= 2;
    return vector;
  };
  vector<int> kernel0Variables{0,1};

  // Kernel1 refreshes associated variables to ~ Unif[0, 1].
  auto kernel1 = [&mockRng1] (typename State::DynamicRealVector vector) {
    for (int i = 0; i < vector.size(); i++) {
      vector[i] = mockRng1.getUnif01RandomVariable();
    }
    return vector;
  };
  vector<int> kernel1Variables{2,3};

  return {
    make_shared<MarkovKernelNode<State, decltype(kernel0)>>(
      kernel0Variables, kernel0),
    make_shared<MarkovKernelNode<State, decltype(kernel1)>>(
      kernel1Variables, kernel1)
  };
}

std::vector<PtrToVariableNode> setUpVariableNodes() {
  return {
    make_shared<VariableNode>(vector<int>{0}),
    make_shared<VariableNode>(vector<int>{}),
    make_shared<VariableNode>(vector<int>{0}),
    make_shared<VariableNode>(vector<int>{})
  };
}

std::vector<PtrToFactorNode> setUpFactorNodes(
  MockRNG& mockRng0, MockRNG& mockRng1) {

  // Factor0 always returns time 1.0f.
  // We control rejection step by controlling the mockRng0.
  auto factor0In = [] (const auto& subvector, const auto& host) {
    // Just return some number in [0, 1].
    if (subvector.norm() > 1.0f) {
      return 1.0f / subvector.norm();
    }
    return subvector.norm();
  };
  auto factor0Pp =
    [&mockRng0] (const auto& subvector, auto& host, auto& fullState) {

    auto time = 1.0f;
    float unif = mockRng0.getUnif01RandomVariable();
    auto thinningStep =
      [time, &host, unif, fullState] () {
        auto realIntensity = host.evaluateIntensity(fullState, time);
        bool shouldAccept = unif < realIntensity;
        return shouldAccept;
    };
    return make_shared<PoissonProcessResult<decltype(thinningStep)>>(
      time, thinningStep);
  };
  vector<int> factor0Ids{0};

  // Factor1. This is simply an exp(1) inhomogeneous Poisson process factor.
  auto factor1Pp =
    [&mockRng1] (const auto& subvector, const auto& host, auto&) {

    return make_shared<PoissonProcessResult<>>(
      (-1.0f) * log(mockRng1.getUnif01RandomVariable()));
  };
  vector<int> factor1Ids{};

  // Factor0 depends on Factor1
  // Factor1 depends only on itself.
  return {
    make_shared<FactorNode<State, decltype(factor0Pp), LinearFlow, decltype(factor0In)>>(
      factor0Ids, factor0Pp, factor0In),
    make_shared<FactorNode<State, decltype(factor1Pp)>>(
      factor1Ids, factor1Pp)
  };
}

}

TEST(PdmpIntegrationTests, TestSimpleProcessExecutesCorrectly) {
  MockRNG kernel0Rng, kernel1Rng, factor0Rng, factor1Rng;
  auto kernels = setUpMarkovKernelNodes(kernel0Rng, kernel1Rng);
  auto variables = setUpVariableNodes();
  auto factors = setUpFactorNodes(factor0Rng, factor1Rng);
  auto graph = make_shared<MyDependenciesGraph>(kernels, variables, factors);
  MyPdmp pdmp(make_tuple(graph), make_tuple(graph));

  State initialState({0.2f, 0.3f}, {1.0f, 2.0f});

  // Set kernel1, to always reset both velocity variables to 0.5f;
  EXPECT_CALL(kernel1Rng, getUnif01RandomVariable())
    .Times(4)
    .WillRepeatedly(Return(0.5f));

  // Set factor0 thinning results.
  EXPECT_CALL(factor0Rng, getUnif01RandomVariable())
    .Times(4)
    .WillOnce(Return(1.0f)) // Reject the first proposal at time t = 1.
    .WillOnce(Return(0.0f)) // Accept the second proposal at time t = 2.
    .WillOnce(Return(0.0f)) // Would accept (t = 3), but this should be
                            // resimulated due to factor1.
    .WillRepeatedly(Return(1.0f)); // Always reject further (starting from
                                   // t = 3).

  // Set factor1 (i.e. Exp(1) homogemeous Poisson process) returns.
  EXPECT_CALL(factor1Rng, getUnif01RandomVariable())
    .Times(2)
    .WillOnce(Return(exp(-2.5f))) // The first time of factor1 is 2.5.
    .WillOnce(Return(exp(-0.9f))); // The next proposed time will be 3.4.

  // Kernel0 should be invoked at t = 2, which multiplies position variables
  // by 2.
  State expectedIntermediateState =
    LinearFlow::advanceStateByFlow(initialState, 2.0f);
  State expectedStateAfterFirstIteration =
    expectedIntermediateState.constructStateWithModifiedVariables(
      vector<int>{0, 1}, vector<float>{4.4f, 8.6f});
  IterationResult<State> expectedFirstIterationResult(
    expectedStateAfterFirstIteration, 2.0f);

  auto firstIterationResult = pdmp.simulateOneIteration(initialState);
  EXPECT_TRUE(firstIterationResult == expectedFirstIterationResult);

  // Now Factor1 should return at time t = 2.5f.

  State expectedIntermediateState2 =
    LinearFlow::advanceStateByFlow(firstIterationResult.state, 0.5f);
  State expectedStateAfterSecondIteration =
    expectedIntermediateState2.constructStateWithModifiedVariables(
      std::vector<int>{2, 3}, std::vector<float>{0.5f, 0.5f});
  IterationResult<State> expectedSecondIterationResult(
    expectedStateAfterSecondIteration, 0.5f);

  auto secondIterationResult = pdmp.simulateOneIteration(
    firstIterationResult.state);
  EXPECT_TRUE(secondIterationResult == expectedSecondIterationResult);

  // Now we test if factor0 time proposed at t = 3 will be correctly
  // invalidated, even though would be accepted.
  State expectedIntermediateState3 =
    LinearFlow::advanceStateByFlow(secondIterationResult.state, 0.9f);
  State expectedStateAfterThirdIteration =
    expectedIntermediateState3.constructStateWithModifiedVariables(
      std::vector<int>{2, 3}, std::vector<float>{0.5f, 0.5f});
  IterationResult<State> expectedThirdIterationResult(
    expectedStateAfterThirdIteration, 0.9f);

  auto thirdIterationResult = pdmp.simulateOneIteration(
    secondIterationResult.state);
  EXPECT_TRUE(thirdIterationResult == expectedThirdIterationResult);

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

