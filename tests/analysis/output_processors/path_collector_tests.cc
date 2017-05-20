
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "analysis/output_processors/path_collector.h"
#include "core/pdmp.h"

#include "../mock_pdmp.h"
#include "output_processors_tests_utils.h"

using namespace std;
using namespace myutils;
using namespace pdmp;
using namespace pdmp::analysis;

TEST(PathCollectorTests, TestSimplePathCollection) {
  PathCollector<MockPdmp, State> pathCollector;
  pathCollector.notifyProcessBegins(MockPdmp(), State{1.0f});
  pathCollector.notifyIterationResult(
    IterationResult<State>{State{2.0f}, 0.0f});
  pathCollector.notifyIterationResult(
    IterationResult<State>{State{3.0f}, 0.0f});

  vector<State> collectedOutput = pathCollector.getCollectedStates();
  EXPECT_EQ(3, collectedOutput.size());
  EXPECT_FLOAT_EQ(1.0f, collectedOutput[0].x);
  EXPECT_FLOAT_EQ(2.0f, collectedOutput[1].x);
  EXPECT_FLOAT_EQ(3.0f, collectedOutput[2].x);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
