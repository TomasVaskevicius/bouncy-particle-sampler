#pragma once

#include <gmock/gmock.h>

#include <vector>

class MockTimer {
 public:
  static MockTimer& getTimer();
  MOCK_METHOD0(start, void());
  MOCK_METHOD0(stop, void());
  MOCK_METHOD0(getTimeInMs, long());
};

// The timers will be returned sequentially from the following vector of timers.
// After each test case, the timers vectors should be cleared and the
// nextTimerToReturn variable should be set to 0.
std::vector<MockTimer*> timersPool;
int nextTimerToReturn = 0;

MockTimer& MockTimer::getTimer() {
  nextTimerToReturn++;
  return *timersPool.at(nextTimerToReturn - 1);
}
