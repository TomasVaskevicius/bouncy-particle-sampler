#include <limits>
#include <memory>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Eigen/Core>

#include "core/dependencies_graph/markov_kernel_node.h"
#include "core/dependencies_graph/factor_node.h"


using DynamicRealVector = Eigen::Matrix<float, Eigen::Dynamic, 1>;

template<int Dim>
using RealVector = Eigen::Matrix<float, Dim, 1>;

using pdmp::dependencies_graph::MarkovKernelNode;
using pdmp::dependencies_graph::FactorNode;
using namespace testing;

const int kStateSpaceDim = 4;

/**
 * State space implementation for testing.
 */
struct DummyState {

  using RealType = float;

  DummyState(const RealVector<kStateSpaceDim>& internalVector)
    : internalVector(internalVector) {
  }

  DynamicRealVector getSubvector(const std::vector<int> ids) const{
    DynamicRealVector subvector(ids.size());
    for (int i = 0; i < ids.size(); i++) {
      subvector[i] = this->internalVector[ids[i]];
    }
    return subvector;
  }

  template<class VectorType>
  DummyState constructStateWithModifiedVariables(
    const std::vector<int>& ids, VectorType modification) const {

    RealVector<kStateSpaceDim> internalVectorCopy = internalVector;
    for (int i = 0; i < ids.size(); i++) {
      internalVectorCopy[ids[i]] = modification[i];
    }
    return DummyState(internalVectorCopy);
  }


  template<class VectorType>
  void modifyStateInPlace(
    const std::vector<int>& ids, VectorType modification) {

    for (int i = 0; i < ids.size(); i++) {
      this->internalVector[ids[i]] = modification[i];
    }
  }

  RealVector<kStateSpaceDim> internalVector;
};

struct NoOpFlow {
  template<class State, typename RealType>
  static State advanceStateByFlow(State state, RealType time) {
    return state;
  };
};

bool operator==(const DummyState& lhs, const DummyState& rhs) {
  return lhs.internalVector.isApprox(rhs.internalVector);
}

namespace pdmp {
namespace dependencies_graph {
struct PoissonProcessResultBase {
  PoissonProcessResultBase(const float& time) : time(time) {}
  float time;
};
}
}
using pdmp::dependencies_graph::PoissonProcessResultBase;
using PoissonProcessResultPtr = std::shared_ptr<PoissonProcessResultBase>;

const auto dummyLambda = [] (auto, const auto&, auto&) -> PoissonProcessResultPtr {};
const DummyState dummyState(RealVector<kStateSpaceDim>(0.0f, 0.0f, 0.0f, 0.0f));


TEST(MarkovKernelNodeUnitTests, TestLambdaFunctionIsInvokedCorrectly) {
  // Set up a Markov kernel node.
  const std::vector<int> ids{0,3};
  auto markovKernel = [] (RealVector<2> vector) -> RealVector<2> {
    return vector * 2;
  };
  MarkovKernelNode<DummyState, decltype(markovKernel)>
    markovKernelNode(ids, markovKernel);

  // Set up initial state.
  const RealVector<kStateSpaceDim> initialVector(1.0f, 2.0f, 3.0f, 4.0f);
  DummyState initialState(initialVector);

  // Set up modified state.
  const RealVector<kStateSpaceDim> expectedVector(2.0f, 2.0f, 3.0f, 8.0f);
  DummyState expectedState(expectedVector);

  EXPECT_TRUE(markovKernelNode.jump(initialState) == expectedState);
}

TEST(FactorNodeTests, TestIntensityCalculationsAreCorrect) {
  // Set up a factor node.
  const std::vector<int> ids{1,3};
  auto factor = [] (auto subvector, const auto& host) -> float {
    return subvector.norm();
  };
  FactorNode<DummyState, decltype(dummyLambda), NoOpFlow, decltype(factor)>
    factorNode(ids, dummyLambda, factor);

  // Set up initial state.
  const RealVector<kStateSpaceDim> initialVector(1.0f, 3.0f, 3.0f, 4.0f);
  DummyState initialState(initialVector);

  const float expectedResult = 5.0f; // sqrt(3^2 + 4^2)

  EXPECT_TRUE(
    std::abs(factorNode.evaluateIntensity(initialState) - expectedResult)
    < std::numeric_limits<float>::min());
}

TEST(FactorNodeTests, TestNoOpIntensityThrowsAnException) {
  const std::vector<int> ids{};
  FactorNode<DummyState, decltype(dummyLambda), NoOpFlow>
    factorNode(ids, dummyLambda);

  EXPECT_THROW(factorNode.evaluateIntensity(dummyState), std::runtime_error);
}

TEST(FactorNodeTests, TestPoissonProcessLambdaCalculationsAreCorrect) {
  // Set up the Poisson process calculation lambda.
  const std::vector<int> ids{0,1};
  auto poissonProcessLambda = [] (auto subvector, const auto& host, auto&) {
    return std::make_shared<PoissonProcessResultBase>(subvector.norm());
  };
  FactorNode<DummyState, decltype(poissonProcessLambda), NoOpFlow> factorNode(
    ids, poissonProcessLambda);

  // Set up initial state.
  const RealVector<kStateSpaceDim> vector(8.0f, 6.0f, 1.0f, 1.0f);
  DummyState state(vector);

  const double expectedResult = 10.0f;

  EXPECT_TRUE(
    std::abs(factorNode.getPoissonProcessResult(state)->time - expectedResult)
    < std::numeric_limits<float>::min());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

