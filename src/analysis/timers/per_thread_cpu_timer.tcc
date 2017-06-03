#pragma once

namespace pdmp {
namespace analysis {

PerThreadCpuTimer PerThreadCpuTimer::getTimer() {
  return PerThreadCpuTimer();
}

timespec PerThreadCpuTimer::getTime() {
  timespec time;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
  return time;
}

}
}
