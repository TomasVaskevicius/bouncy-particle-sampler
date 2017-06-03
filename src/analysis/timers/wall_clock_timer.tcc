#pragma once

namespace pdmp {
namespace analysis {

WallClockTimer WallClockTimer::getTimer() {
  return WallClockTimer();
}

timespec WallClockTimer::getTime() {
  timespec time;
  clock_gettime(CLOCK_MONOTONIC, &time);
  return time;
}

}
}
