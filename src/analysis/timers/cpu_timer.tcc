#pragma once

namespace pdmp {
namespace analysis {

CpuTimer CpuTimer::getTimer() {
  return CpuTimer();
}

timespec CpuTimer::getTime() {
  timespec time;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time);
  return time;
}

}
}
