#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>

#include "analysis/output_processors/autocorrelation_calculator.h"
#include "core/pdmp.h"

#include "../mock_pdmp.h"
#include "output_processors_tests_utils.h"

using namespace std;
using namespace myutils;
using namespace pdmp;
using namespace pdmp::analysis;

using MyAutocorrelationCalculator
  = AutocorrelationCalculator<MockPdmp, State, SimpleFlow>;
using MyTimeWindow = MyAutocorrelationCalculator::TimeWindow;

// Node, constants kMaxLag, kStepsPerUnitLength, kWindowSize and
// kWaitForTrajectoryLength are all defined in autocorrelation_calculator.tcc.

float kStepSize = 1.0f / kStepsPerUnitLength;

// A one dimensional function given by f(x) = x.
std::function<float(State)> linearFunction = [] (State state) -> float {
  return state.x;
};

// A function cyclic with period kMaxLag.
// It is shaped of many tents, going from 0 to 1 and then to 0 again.
std::function<float(State)> cyclicFunction = [] (State state) -> float {
  float x = state.x;
  while (x > kMaxLag || areEqual(x, kMaxLag)) {
    x -= kMaxLag;
  }
  while (x < 0) {
    x += kMaxLag;
  }
  // Now x is in [0, kMaxLag).
  if (x <= kMaxLag / 2.0f) {
    return x / (kMaxLag / 2.0f);
  } else {
    return 2.0f - (x / (kMaxLag / 2.0f));
  }
};


// A test fixture class for the time window tests.
class TimeWindowTests : public ::testing::Test {

 protected:

  // Given continuous segments, adds them to the time window.
  void addSegments(
    vector<IterationResult<State>> endpoints,
    std::function<float(State)>& integrand) {

    for (int i = 1; i < endpoints.size(); i++) {
      timeWindow_.addSegment(endpoints[i-1], endpoints[i], integrand);
    }
  }

  // Checks if the integrand estimates held by the timeWindow_ mathches
  // out expectations.
  void testExpectedIntegrandEstimates(vector<float> expectedEstimates) {
    auto realIntegrandEvaluations = timeWindow_.getIntegrandAtPoints();
    EXPECT_TRUE(realIntegrandEvaluations.size() == expectedEstimates.size());
    for (int i = 0; i < expectedEstimates.size(); i++) {
      EXPECT_TRUE(areEqual(expectedEstimates[i], realIntegrandEvaluations[i]));
    }
  }

  MyTimeWindow timeWindow_;
};

TEST_F(TimeWindowTests, TestSegmentInsertionWithoutPadding) {
  addSegments(
    {
      IterationResult<State>{State{0.0f}, 0},
      IterationResult<State>{State{kStepSize}, kStepSize},
      IterationResult<State>{State{kStepSize * 3}, 2 * kStepSize}
    },
    linearFunction);
  testExpectedIntegrandEstimates({kStepSize, kStepSize * 2, kStepSize * 3});
}

TEST_F(TimeWindowTests, TestSegmentInsertionWithPadding) {
  addSegments(
    {
      IterationResult<State>{State{0.0f}, 0},
      IterationResult<State>{State{kStepSize * 1.5f}, kStepSize * 1.5f},
      IterationResult<State>{State{kStepSize * 3}, 1.5f * kStepSize}
    },
    linearFunction);
  testExpectedIntegrandEstimates({kStepSize, kStepSize * 2, kStepSize * 3});
}

TEST_F(TimeWindowTests, TestSegmentInsertionWithPadding2) {
  addSegments(
    {
      IterationResult<State>{State{0.0f}, 0},
      IterationResult<State>{State{kStepSize * 1.5f}, kStepSize * 1.5f},
    },
    linearFunction);
  testExpectedIntegrandEstimates({kStepSize});
}

TEST_F(TimeWindowTests, TestCovarianceCalculationForCyclicFunction) {
  addSegments(
    {
      IterationResult<State>{State{0.0f}, 0},
      IterationResult<State>{State{kStepSize * kWindowSize * 2},
                             kStepSize * kWindowSize * 2}
    },
    cyclicFunction);
  vector<float> covarianceEstimates(kWindowSize, 0.0f);
  int numberOfPointsUsed = 0;
  timeWindow_.updateCovarianceEstimatesAndCleanUpData(
    0.5f, covarianceEstimates, numberOfPointsUsed);


  // Check if correct number of points were used for covariance estimates.
  // Note that the first kWindowSize points are supposed to be the
  // "remainder" from previous calculation and they are not counted.
  EXPECT_TRUE(numberOfPointsUsed == kWindowSize);
  // Variance of our Marko chain is 1/12.
  float correlationAtPeriod = covarianceEstimates.at(kWindowSize - 1)
    * 12 / numberOfPointsUsed;
  float correlationAtHalfPeriod = covarianceEstimates.at((kWindowSize / 2) - 1)
    * 12 / numberOfPointsUsed;
  EXPECT_TRUE(areEqual(correlationAtPeriod, 1.0f, 1e-2));
  EXPECT_TRUE(areEqual(correlationAtHalfPeriod, -1.0f, 1e-2));
}

TEST(
  AutocorrelationCalculatorIntegrationTests,
  TestAutocorrelationCalculationForCyclicFunction) {

  MyAutocorrelationCalculator autocorrelationCalculator(cyclicFunction);
  autocorrelationCalculator.notifyProcessBegins(MockPdmp(), State{0.0f});
  autocorrelationCalculator.notifyIterationResult(
    IterationResult<State>{State{kStepSize * kWindowSize * 2},
                           kStepSize * kWindowSize * 2});

  auto autocorrelations = autocorrelationCalculator.getAutocorrelationsVector();
  // Note the indexing is shifted by one here, since autocorrelationsCalculator
  // adds lag 0 autocorrelation (for plotting).
  float correlationAtPeriod = autocorrelations.at(kWindowSize);
  float correlationAtHalfPeriod = autocorrelations.at((kWindowSize / 2));
  EXPECT_TRUE(areEqual(correlationAtPeriod, 1.0f, 1e-2));
  EXPECT_TRUE(areEqual(correlationAtHalfPeriod, -1.0f, 1e-2));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

