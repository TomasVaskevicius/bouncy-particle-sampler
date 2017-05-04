#include <gtest/gtest.h>

#include "core/state_space/position_and_velocity_state.h"

/**
 * Here we test our state space representation functionalities.
 */

/**
 * Test fixture holding a simple state.
 */
class PositionAndVelocityStateTests : public ::testing::Test {

 protected:

  using State = pdmp::PositionAndVelocityState<float, 4>;
  using RealVector = State::RealVector<2>;

  PositionAndVelocityStateTests()
    : state_(*(new RealVector(1.0f, 2.0f)), *(new RealVector(3.0f, 4.0f))) {
  }

  const State state_;
};

TEST_F(PositionAndVelocityStateTests, TestCorrectPositionElementExtraction) {
  EXPECT_TRUE(state_.getElementAtIndex(1) == 2.0f);
}

TEST_F(PositionAndVelocityStateTests, TestCorrectVelocityElementExtraction) {
  EXPECT_TRUE(state_.getElementAtIndex(3) == 4.0f);
}

TEST_F(PositionAndVelocityStateTests, TestOutOfBoundsElementExtraction) {
  EXPECT_THROW(state_.getElementAtIndex(-1), std::out_of_range);
 }

TEST_F(PositionAndVelocityStateTests, TestOutOfBoundsElementExtraction2) {
  EXPECT_THROW(state_.getElementAtIndex(4), std::out_of_range);
}

TEST_F(PositionAndVelocityStateTests, TestCorrectStateModification) {
  std::vector<int> ids{1,2};
  std::vector<float> modification{0.5f, 1.5f};

  RealVector expectedPosition(1.0f, 0.5f);
  RealVector expectedVelocity(1.5f, 4.0f);
  State expectedState(expectedPosition, expectedVelocity);

  EXPECT_TRUE(
    state_.constructStateWithModifiedVariables(ids, modification)
    == expectedState);
}

TEST_F(PositionAndVelocityStateTests, TestInvalidStateModification) {
  std::vector<int> ids{1,2};
  std::vector<float> modification{1};
  // Ids and modification should be of the same size.
  EXPECT_THROW(
    state_.constructStateWithModifiedVariables(ids, modification),
    std::logic_error);
}

TEST_F(PositionAndVelocityStateTests, TestEmptySubvectorExtractionFails) {
  std::vector<int> ids;
  EXPECT_THROW(state_.getSubvector(ids), std::logic_error);
}

TEST_F(PositionAndVelocityStateTests, TestTooBigSubvectorExtractionFails) {
  std::vector<int> ids{0,1,2,3,4,5};
  EXPECT_THROW(state_.getSubvector(ids), std::logic_error);
}

TEST_F(PositionAndVelocityStateTests, TestCorrectSubvectorExtraction) {
  std::vector<int> ids{1,3};
  State::DynamicRealVector expectedVector(2);
  expectedVector <<
    state_.getElementAtIndex(ids[0]), state_.getElementAtIndex(ids[1]);
  EXPECT_TRUE(expectedVector.isApprox(state_.getSubvector(ids)));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

