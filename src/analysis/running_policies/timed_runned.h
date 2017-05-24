#pragma once

namespace pdmp {
namespace analysis {

template<class Timer>
class TimedRunner {

 protected:

  /**
   * The running policy implementation constrained by ceraint time conditions.
   * Should be called by the PdmpRunner host class.
   *
   * @param
   * @param pdmp
   *   The Pdmp that we want to run.
   * @param initialState
   *   The state used to start our process.
   * @param runningTime
   *   Time in ms after which the process will be stopped.
   * @param burnInTime
   *   Time in ms for which we run the process before starting the actual run.
   *   This time is not counted into the running time.
   *   The observers are not notified about the iterations during this period.
   * @param excludeObserverTimes
   *   A boolean flag indicating whether we should exclude the time spent
   *   to notify observers (possibly waiting for their calculations to finish)
   *   from the total running time.
   */
  template<class PdmpRunner, class Pdmp, class State>
  void run(
    PdmpRunner& pdmpRunnerHost,
    Pdmp& pdmp,
    State&& initialState,
    long runningTime,
    long burnInTime = 0,
    bool excludeObserverTimes = true);

};

}
}

#include "timed_runned.tcc"
