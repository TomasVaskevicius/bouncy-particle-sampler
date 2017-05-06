#include <limits>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Eigen/Core>

#include "core/dependencies_graph/nodes.h"

const int kStateSpaceDim = 4;

using DynamicRealVector = Eigen::Matrix<float, Eigen::Dynamic, 1>;

template<int Dim>
using RealVector = Eigen::Matrix<float, Dim, 1>;

using pdmp::dependencies_grap::MarkovKernelNode;
using pdmp::dependencies_grap::FactorNode;
using namespace testing;

/**
 * State space implementation for testing.
 */
struct DummyState {

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

  const RealVector<kStateSpaceDim> internalVector;
};

bool operator==(const DummyState& lhs, const DummyState& rhs) {
  return lhs.internalVector.isApprox(rhs.internalVector);
}

TEST(MarkovKernelNodeUnitTests, TestLambdaFunctionIsInvokedCorrectly) {
  // Set up a Markov kernel node.
  const std::vector<int> ids{0,3};
  auto markovKernel = [] (RealVector<2> vector) -> RealVector<2> {
    return vector * 2;
  };
  MarkovKernelNode<decltype(markovKernel)> markovKernelNode(ids, markovKernel);

  // Set up initial state.
  const RealVector<kStateSpaceDim> initialVector(1.0f, 2.0f, 3.0f, 4.0f);
  DummyState initialState(initialVector);

  // Set up modified state.
  const RealVector<kStateSpaceDim> expectedVector(2.0f, 2.0f, 3.0f, 8.0f);
  DummyState expectedState(expectedVector);

  EXPECT_TRUE(markovKernelNode.jump(initialState) == expectedState);
}

TEST(FactorNodeTests, TestFactorNodeLambdaCalculationsAreCorrect) {
  // Set up a factor node.
  const std::vector<int> ids{1,3};
  auto factor = [] (DynamicRealVector vector) -> float {
    return vector.norm();
  };
  FactorNode<decltype(factor)> factorNode(ids, factor);

  // Set up initial state.
  const RealVector<kStateSpaceDim> initialVector(1.0f, 3.0f, 3.0f, 4.0f);
  DummyState initialState(initialVector);

  const float expectedResult = 5.0f; // sqrt(3^2 + 4^2)

  EXPECT_TRUE(
    std::abs(factorNode.evaluateIntensity(initialState) - expectedResult)
    < std::numeric_limits<float>::min());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

