#pragma once

namespace {

// Runs the given pdmp for a specified amount of time.
// Returns the last state.
template<class Timer, class Pdmp, class State>
State burnIn(
  Pdmp& pdmp,
  const State& initialState,
  long burnInTime) {

  decltype(auto) timer = Timer::getTimer();
  State lastState = initialState;
  while (timer.getTimeInMs() < burnInTime) {
    timer.start();
    auto iterationResult = pdmp.simulateOneIteration(lastState);
    lastState = iterationResult.state;
    timer.stop();
  }
  return lastState;
}

}

namespace pdmp {
namespace analysis {

template<class Timer>
template<class PdmpRunner, class Pdmp, class State>
void TimedRunner<Timer>::run(
  PdmpRunner& pdmpRunnerHost,
  Pdmp& pdmp,
  const State& initialState,
  long runningTime,
  long burnInTime,
  bool excludeObserverTimes) {

  State lastState = burnIn<Timer, Pdmp, State>(pdmp, initialState, burnInTime);
  static_cast<PdmpRunner*>(this)->signalProcessBegins(pdmp, lastState);

  decltype(auto) timer = Timer::getTimer();
  while (timer.getTimeInMs() < runningTime) {
    timer.start();
    auto iterationResult = pdmp.simulateOneIteration(lastState);
    lastState = iterationResult.state;
    if (excludeObserverTimes) {
      timer.stop();
      static_cast<PdmpRunner*>(this)->signalIterationResult(iterationResult);
    } else {
      static_cast<PdmpRunner*>(this)->signalIterationResult(iterationResult);
      timer.stop();
    }
  }

  static_cast<PdmpRunner*>(this)->signalProcessEnded();
}


}
}
