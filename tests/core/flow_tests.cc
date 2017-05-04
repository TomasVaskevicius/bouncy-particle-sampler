#include <gtest/gtest.h>

#include "core/flow/linear_flow.h"
#include "core/state_space/position_and_velocity_state.h"

/**
 * The purpose of this test suite is to test the flow policy implementations
 * located at the src/core directory.
 */

/**
 * A simple test to check if LinearFlow calculations work correct.
 * We use it with State having a position and velocity. After time t,
 * the position should become position + velocity * t, while the
 * velocity should remain unchanged.
 */
TEST(LinearFlowTest, TestLinearFlowCalculationsAreCorrect) {
  const int dimension = 6;
  using State = pdmp::PositionAndVelocityState<float, dimension>;
  using RealVector = State::RealVector<dimension / 2>;

  const RealVector initialPoisition(1.0f, 2.0f, 3.0f);
  const RealVector initialVelocity(2.5f, 3.25f, 5.0f);
  const int time = 2.0f;

  const RealVector expectedPosition(6.0f, 8.5f, 13.0f);
  const RealVector expectedVelocity = initialVelocity;

  const State initialState(initialPoisition, initialVelocity);
  const State expectedState(expectedPosition, expectedVelocity);
  const State actualState = pdmp::LinearFlow::advanceStateByFlow(
    initialState, time);

  EXPECT_TRUE(actualState == expectedState);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

