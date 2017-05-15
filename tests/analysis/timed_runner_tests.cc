#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "analysis/pdmp_runner.h"
#include "analysis/running_policies/timed_runned.h"

#include "mock_pdmp.h"
#include "mock_timer.h"

using namespace pdmp;
using namespace pdmp::analysis;
using namespace testing;

using MyPdmpRunner = PdmpRunner<MockPdmp, DummyState, TimedRunner<MockTimer>>;

const IterationResult<DummyState> kDummyIterationResult(DummyState(0), 0.0f);

namespace {

// Prepares the timer for a given number of iterations.
void setUpTimer(MockTimer& timer, int numberOfCalls) {
  if (numberOfCalls > 0) {
    EXPECT_CALL(timer, start()).Times(numberOfCalls);
    EXPECT_CALL(timer, stop()).Times(numberOfCalls);
    {
      InSequence s;
      // Returning -1 from the timer will signal that the timer hasn't finished
      // yet.
      EXPECT_CALL(timer, getTimeInMs())
        .Times(numberOfCalls)
        .WillRepeatedly(Return(-1));
      // Returning 1 will terminate the process.
      EXPECT_CALL(timer, getTimeInMs())
        .Times(1)
        .WillOnce(Return(1)); // Signals that the process should terminate.
    }
  } else {
    EXPECT_CALL(timer, getTimeInMs())
      .Times(1)
      .WillOnce(Return(1));
  }
}

}

class TimedRunnerTests : public ::testing::Test {

 protected:

  TimedRunnerTests() {
    nextTimerToReturn = 0;
    timersPool = std::vector<MockTimer*>{&burnInTimer_, &runningTimer_};
  }

  void setUpBurnInCalls(int numberOfCalls) {
    numberOfCallsSetUp += numberOfCalls;
    setUpTimer(burnInTimer_, numberOfCalls);
  }

  void setUpMainCalls(int numberOfCalls) {
    numberOfCallsSetUp += numberOfCalls;
    setUpTimer(runningTimer_, numberOfCalls);
  }

  void run() {
    if (numberOfCallsSetUp > 0) {
      EXPECT_CALL(pdmp_, simulateOneIteration(_))
        .Times(numberOfCallsSetUp)
        .WillRepeatedly(Return(kDummyIterationResult));
    }

    runner_.run(pdmp_, dummyState_, 0);
  }

  MockTimer burnInTimer_;
  MockTimer runningTimer_;
  DummyState dummyState_{0};

 private:
  int numberOfCallsSetUp{0};
  MyPdmpRunner runner_;
  MockPdmp pdmp_;
};

TEST_F(TimedRunnerTests, TestBurnInExecutesCorrectly) {
  setUpBurnInCalls(2);
  setUpMainCalls(0);
  run();
}


TEST_F(TimedRunnerTests, TestMainLoopExecutesCorrectly) {
  setUpBurnInCalls(0);
  setUpMainCalls(1);
  run();
}

TEST_F(TimedRunnerTests, TestBurnInAndMainLoopBothExecuteCorrectly) {
  setUpBurnInCalls(2);
  setUpMainCalls(3);
  run();
}

TEST_F(TimedRunnerTests, TestZeroTimes) {
  setUpBurnInCalls(0);
  setUpMainCalls(0);
  run();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

