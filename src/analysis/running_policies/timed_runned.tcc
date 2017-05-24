#pragma once

#include <type_traits>
#include <utility>

namespace {

// Runs the given pdmp for a specified amount of time.
// Returns the last state.
template<class Timer, class Pdmp, class State>
auto burnIn(
  Pdmp& pdmp,
  const State& initialState,
  long burnInTime) {

  decltype(auto) timer = Timer::getTimer();
  State lastState = initialState;
  while (timer.getTimeInMs() < burnInTime) {
    timer.start();
    auto iterationResult = pdmp.simulateOneIteration(std::move(lastState));
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
  State&& initialState,
  long runningTime,
  long burnInTime,
  bool excludeObserverTimes) {

  using State_t = std::decay_t<State>;

  State_t lastState = burnIn<Timer, Pdmp, State_t>(
    pdmp, std::forward<State>(initialState), burnInTime);
  static_cast<PdmpRunner*>(this)->signalProcessBegins(pdmp, lastState);

  decltype(auto) timer = Timer::getTimer();
  while (timer.getTimeInMs() < runningTime) {
    timer.start();
    auto iterationResult = pdmp.simulateOneIteration(std::move(lastState));
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
