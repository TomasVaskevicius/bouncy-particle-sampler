#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <limits>

#include "analysis/output_processors/batch_means.h"
#include "core/pdmp.h"

#include "../mock_pdmp.h"

using namespace std;
using namespace pdmp;
using namespace pdmp::analysis;

struct State {
  using RealType = float;
  float x;
};

struct MyFlow {
  static State advanceStateByFlow(State state, float time) {
    return State{state.x + time};
  }
};

using BatchMeansProcessor = BatchMeans<MockPdmp, State, MyFlow>;

auto constIntegrand = [] (State s) -> float { return 1.0f; };

bool areEqual(float lhs, float rhs) {
  return fabs(lhs - rhs) < 1e-7;
}

bool testBatchLengths(
  const vector<Batch<float>>& batches,
  const vector<float>& expectedLengths) {

  EXPECT_TRUE(batches.size() == expectedLengths.size());
  for (int i = 0; i < batches.size(); i++) {
    EXPECT_TRUE(areEqual(batches[i].trajectoryLength, expectedLengths[i]));
  }
}

TEST(BatchNumberTests, TestFirstBatchSaturation) {
  BatchMeansProcessor processor(constIntegrand);
  State initalState{0.0f};
  processor.notifyProcessBegins(MockPdmp(), initalState);
  processor.notifyIterationResult({State{}, 1.0f});
  testBatchLengths(processor.getBatches(), {1.0f, 0.0f});
}

TEST(BatchNumberTests, TestSecondBatchIsCreatedAsAppropriate) {
  BatchMeansProcessor processor(constIntegrand);
  State initalState{0.0f};
  processor.notifyProcessBegins(MockPdmp(), initalState);
  processor.notifyIterationResult({State{}, 0.5f});
  processor.notifyIterationResult({State{}, 0.6f});
  testBatchLengths(processor.getBatches(), {1.0f, 0.1f});
}

TEST(BatchNumberTests, TestCorrectBatchCreationForLongTrajectory) {
  BatchMeansProcessor processor(constIntegrand);
  State initalState{0.0f};
  processor.notifyProcessBegins(MockPdmp(), initalState);
  processor.notifyIterationResult({State{}, 64.0f});
  testBatchLengths(
    processor.getBatches(),
    {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 0.0f});
}

// We refer to conditions as perfect, if we have sqrt(T) batches of
// length sqrt(T) where T is the trajectory length.
TEST(
  BatchNumberTests,
  TestBatchesRestructurizationAfterPerfectConditionsAreMet) {

  BatchMeansProcessor processor(constIntegrand);
  State initalState{0.0f};
  processor.notifyProcessBegins(MockPdmp(), initalState);
  processor.notifyIterationResult({State{}, 16.0f});
  testBatchLengths(
    processor.getBatches(),
    {4.0f, 4.0f, 4.0f, 4.0f, 0.0f});

  processor.notifyIterationResult({State{}, 12.0f});
  testBatchLengths(
    processor.getBatches(),
    {8.0f, 8.0f, 8.0f, 4.0f, 0.0f});

  processor.notifyIterationResult({State{}, 15.0f});
  testBatchLengths(
    processor.getBatches(),
    {8.0f, 8.0f, 8.0f, 8.0f, 8.0f, 3.0f});
}

TEST(BatchMeansTest, TestAsymptoticVarianceIsZeroForConstantFunction) {
  BatchMeansProcessor processor(constIntegrand);
  State initalState{0.0f};
  processor.notifyProcessBegins(MockPdmp(), initalState);
  processor.notifyIterationResult({State{1.5f}, 1.5f});
  EXPECT_TRUE(areEqual(processor.estimateMean(), 1.0f));
  EXPECT_TRUE(areEqual(processor.estimateAsymptoticVariance(), 0.0f));
}

TEST(BatchMeansTest, TestBatchMergingsDoNotLoseInformation) {
  BatchMeansProcessor processor(constIntegrand);
  State initalState{0.0f};
  processor.notifyProcessBegins(MockPdmp(), initalState);
  processor.notifyIterationResult({State{1024.0f}, 1024.0f});
  EXPECT_TRUE(processor.getBatches()[0].value = 32.0f);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
