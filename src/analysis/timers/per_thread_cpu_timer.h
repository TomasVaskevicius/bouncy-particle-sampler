#pragma once

#include <time.h>

#include "clock_gettime_wrapper_base.h"

namespace pdmp {
namespace analysis {

/**
 * A timer which measures CPU time per thread.
 *
 * There are two precodintions:
 * 1) Should not be used to time algorithms which spawn their own threads.
 * 2) Your compiler should be using Posix thread model for std::threads
 *    (For g++, for example, you can check it by typing g++ -v).
 */
class PerThreadCpuTimer : public ClockGettimeWrapperBase {

 public:

  /**
   * A factory method for creating an instance of a timer.
   */
  static PerThreadCpuTimer getTimer();

  virtual timespec getTime() override final;

 private:

  // Should be only created through the factory.
  // We do this for the purpose of easy testing of classes using the timer,
  // by simply allowing to mock the timer.
  PerThreadCpuTimer() = default;
};

}
}

#include "per_thread_cpu_timer.tcc"
