#include <tuple>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/pdmp.h"
#include "mock_policies.h"

/**
 * We will here test the behavious of the host class for assembling
 * piecewise-deterministic Markov processes.
 */


using namespace testing;

TEST(PdmpHostClassTests, TestPdmpWithMockPolicies) {
  DummyPdmp pdmp;
  const float jumpTime = 10.0f;
  const DummyState initialState(0);
  const DummyState stateAfterFlow(1);
  const DummyState stateAfterJump(2);

  using ResultType = pdmp::IterationResult<DummyState>;
  ResultType expectedResult(stateAfterJump, jumpTime);

  // Set up the mock Poisson process policy.
  EXPECT_CALL(
    *static_cast<MockPoissonProcess*>(&pdmp),
    getJumpTime(initialState, _))
      .Times(1)
      .WillOnce(Return(jumpTime));

  // Set up the mock flow policy.
  EXPECT_CALL(
    *static_cast<MockFlow*>(&pdmp),
    advanceStateByFlow(initialState, jumpTime))
      .Times(1)
      .WillOnce(Return(stateAfterFlow));

  // Set up the mock Markov kernel policy.
  EXPECT_CALL(
    *static_cast<MockMarkovKernel*>(&pdmp), jump(stateAfterFlow, _))
      .Times(1)
      .WillOnce(Return(stateAfterJump));

  auto actualResult = pdmp.simulateOneIteration(initialState);
  EXPECT_TRUE(actualResult == expectedResult);
}

TEST(PdmpConstructorsTests, TestDefaultConstructor) {
  DummyPdmp pdmp;
  EXPECT_TRUE(pdmp.dummyPoissonProcessVariable == 0);
  EXPECT_TRUE(pdmp.dummyMarkovKernelVariable == 0);
}

TEST(PdmpConstructorsTests, TestConstructorUnwrappingTuples) {
  DummyPdmp pdmp(std::make_tuple(1), std::make_tuple(2));
  EXPECT_TRUE(pdmp.dummyPoissonProcessVariable == 1);
  EXPECT_TRUE(pdmp.dummyMarkovKernelVariable == 2);
}

TEST(PdmpConstructorsTests, TestConstructorWithEmptyTuple) {
  DummyPdmp pdmp(std::make_tuple(1), std::make_tuple());
  EXPECT_TRUE(pdmp.dummyPoissonProcessVariable == 1);
  EXPECT_TRUE(pdmp.dummyMarkovKernelVariable == 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
