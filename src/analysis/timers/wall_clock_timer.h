#pragma once

#include <time.h>

#include "clock_gettime_wrapper_base.h"

namespace pdmp {
namespace analysis {

/**
 * A timer which measures CPU time of the whole process.
 */
class WallClockTimer : public ClockGettimeWrapperBase {

 public:

  /**
   * A factory method for creating an instance of a timer.
   */
  static WallClockTimer getTimer();

  virtual timespec getTime() override final;

 private:

  // Should be only created through the factory.
  // We do this for the purpose of easy testing of classes using the timer,
  // by simply allowing to mock the timer.
  WallClockTimer() = default;
};

}
}

#include "wall_clock_timer.tcc"
