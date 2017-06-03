#pragma once

#include <time.h>

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
class ClockGettimeWrapperBase {

 public:

  /**
   * Derived classes will call clock_gettime with apropriate parameters.
   */
  virtual timespec getTime() = 0;

  /**
   * Starts the timer. Cannot be called two times in a row.
   */
  void start();

  /**
   * Stops the timer.
   */
  void stop();

  /**
   * Resets the timer to 0 and sets it to stopped state.
   */
  void reset();

  /**
   * Returns the currently measured time in milliseconds.
   */
  long getTimeInMs();

 private:

  // A time currently recorded by this timer.
  timespec currentTime_{0, 0};
  bool isStarted_{false};
  timespec lastStart_;
};

}
}

#include "clock_gettime_wrapper_base.tcc"
