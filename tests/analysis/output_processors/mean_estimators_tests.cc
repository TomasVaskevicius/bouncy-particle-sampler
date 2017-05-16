#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "analysis/output_processors/batch_means.h"
#include "analysis/output_processors/quadrature_mean_estimator.h"
#include "core/pdmp.h"
#include "core/state_space/position_and_velocity_state.h"
#include "core/policies/linear_flow.h"

#include "../mock_pdmp.h"
#include "output_processors_tests_utils.h"

using namespace std;
using namespace myutils;
using namespace pdmp;
using namespace pdmp::analysis;

// Simple function f(x) = x.
auto simpleOneDimensionalFunction = [] (State state) -> float {
  return state.x;
};
// Path for integrating the function on (0, 5) U (7, 10).
auto pathForOneDimensionalFunction =
  std::vector<IterationResult<State>>{
    IterationResult<State>{State{0.0f}, 0.0f},
    IterationResult<State>{State{7.0f}, 5.0f},
    IterationResult<State>{State{10.0f}, 3.0f}
  };
const float kExpectedResultOneDimensionalExample = 4.75f;

// Returns the squared norm.
auto simpleTwoDimensionalFunction =
  [] (auto positionAndVelocityState) -> float {
    float positionNorm = positionAndVelocityState.position.norm();
    return positionNorm * positionNorm;
  };
using PVState = PositionAndVelocityState<float, 4>;
auto pathForTwoDimensionalFunction =
  std::vector<IterationResult<PVState>>{
    IterationResult<PVState>{PVState{{1.0f, 1.0f}, {0.2f, 0.3f}}, 0.0f},
    IterationResult<PVState>{PVState{
      {2001.0f, 3001.0f},{1.0f, -10.0f}}, 1000.0f},
    IterationResult<PVState>{PVState{{1976.0f, 2751.0f}, {1.0f, 1.0f}}, 25.0f}
  };
const float kExpectedResultTwoDimensionalExample
  = (1 / 1025.0f) * (43835300.0f + 924811400.0f / 3.0f);


template<class Estimator, class IterationResultType>
void checkEstimatorResults(
  Estimator& estimator,
  const vector<IterationResultType>& iterationResults,
  float expectedMeanEstimate,
  float floatComparisonEpsilon = 1e-5) {

  estimator.notifyProcessBegins(MockPdmp(), iterationResults[0].state);
  for (int i = 1; i < iterationResults.size(); i++) {
    estimator.notifyIterationResult(iterationResults[i]);
  }
  EXPECT_TRUE(
    areEqual(expectedMeanEstimate, estimator.estimateMean(), floatComparisonEpsilon));
}

TEST(BatchMeansMeanEstimatorTests, SimpleOneDimensionalExample) {
  BatchMeans<MockPdmp, State, SimpleFlow>
    estimator(simpleOneDimensionalFunction);
  checkEstimatorResults(
    estimator,
    pathForOneDimensionalFunction,
    kExpectedResultOneDimensionalExample);
}

TEST(BatchMeansMeanEstimatorTests, RealLinearFlowPolicyTwoDimensionalTest) {
  BatchMeans<MockPdmp, PVState, LinearFlow>
    estimator(simpleTwoDimensionalFunction);
  checkEstimatorResults(
    estimator,
    pathForTwoDimensionalFunction,
    kExpectedResultTwoDimensionalExample,
    1e-1); // Allow large error for this large integration task.
}

TEST(QuadratureMeanEstimatorTests, SimpleOneDimensionalExample) {
  QuadratureMeanEstimator<MockPdmp, State, SimpleFlow>
    estimator(simpleOneDimensionalFunction);
  checkEstimatorResults(
    estimator,
    pathForOneDimensionalFunction,
    kExpectedResultOneDimensionalExample);
}

TEST(QuadratureMeanEstimatorTests, RealLinearFlowPolicyTwoDimensionalTest) {
  QuadratureMeanEstimator<MockPdmp, PVState, LinearFlow>
    estimator(simpleTwoDimensionalFunction);
  checkEstimatorResults(
    estimator,
    pathForTwoDimensionalFunction,
    kExpectedResultTwoDimensionalExample,
    1e-1); // Allow large error for this large integration task.
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
