#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "analysis/pdmp_runner.h"
#include "analysis/running_policies/fixed_iterations_count_runner.h"

#include "mock_observer.h"
#include "mock_pdmp.h"

using namespace pdmp;
using namespace pdmp::analysis;
using namespace testing;

const DummyState kDummyState(0);
const IterationResult<DummyState> kDummyIterationResult(kDummyState, 0.0f);

// This is the simples possible policy for running a pdmp.
// It is intended to test the host class, since the policy itself does
// not have any complicated logic.
using SimpleRunningPolicy = FixedIterationsCountRunner;
using MyPdmpRunner = PdmpRunner<MockPdmp, DummyState, SimpleRunningPolicy>;

// A test fixture class for setting up the mock pdmp and the runner.
class PdmpRunnerTests : public ::testing::Test {

 protected:

  void registerAllObservers() {
    for (auto& observer : observers_) {
      runner_.registerAnObserver(observer);
    }
  }

  void addObserverAndSetUpExpectations(
    MockObserver& observer,
    std::vector<IterationResult<DummyState>> expectedIterationResults) {

    EXPECT_CALL(observer, notifyProcessBegins(_, _)).Times(1);

    for (int i = 0;i < expectedIterationResults.size(); i++) {
      EXPECT_CALL(observer, notifyIterationResult(expectedIterationResults[i]))
        .Times(1);
    }

    EXPECT_CALL(observer, notifyProcessEnded()).Times(1);
    observers_.push_back(&observer);
  }

  MockPdmp pdmp_;
  MyPdmpRunner runner_;
  std::vector<MockObserver*> observers_;
};

TEST_F(PdmpRunnerTests, TestSimpleRunningPolicyIteratesPdmpCorrectNumberOfTimes) {
  const int numberOfIterations = 2;
  EXPECT_CALL(pdmp_, simulateOneIteration(_))
    .Times(numberOfIterations)
    .WillRepeatedly(Return(kDummyIterationResult));
  runner_.run(pdmp_, kDummyState, numberOfIterations);
}

TEST_F(PdmpRunnerTests, TestObserversGetNotifiedAtBeginningOfTheProcess) {
  // Set up the output of a pdmp.
  const int numberOfIterations = 2;
  IterationResult<DummyState> firstIterationResult(DummyState(1), 1.0f);
  IterationResult<DummyState> secondIterationResult(DummyState(2), 2.0f);
  EXPECT_CALL(pdmp_, simulateOneIteration(_))
    .Times(numberOfIterations)
    .WillOnce(Return(firstIterationResult))
    .WillOnce(Return(secondIterationResult));

  MockObserver observer1, observer2;
  addObserverAndSetUpExpectations(
    observer1, {firstIterationResult, secondIterationResult});
  addObserverAndSetUpExpectations(
    observer2, {firstIterationResult, secondIterationResult});

  registerAllObservers();

  runner_.run(pdmp_, kDummyState, numberOfIterations);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
