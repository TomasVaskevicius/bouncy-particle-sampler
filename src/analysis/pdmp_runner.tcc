#pragma once

#include <utility>

namespace pdmp {
namespace analysis {

template<class Pdmp, class State>
void ObserverBase<Pdmp,State>::notifyProcessBegins(
  const Pdmp& pdmp, const State& initialState) {
}

template<class Pdmp, class State>
void ObserverBase<Pdmp, State>::notifyIterationResult(
  const IterationResult<State>& iterationResult) {
}

template<class Pdmp, class State>
void ObserverBase<Pdmp,State>::notifyProcessEnded() {
}

template<class Pdmp, class State, class RunningPolicy>
template<class... Args>
void PdmpRunner<Pdmp, State, RunningPolicy>::run(
  Pdmp& pdmp, const State& initialState, Args&&... args) {

  RunningPolicy::run(*this, pdmp, initialState, std::forward<Args>(args)...);
}

template<class Pdmp, class State, class RunningPolicy>
void PdmpRunner<Pdmp, State, RunningPolicy>
  ::registerAnObserver(ObserverPtr observer) {

  this->observers_.push_back(observer);
}


template<class Pdmp, class State, class RunningPolicy>
void PdmpRunner<Pdmp, State, RunningPolicy>::signalProcessBegins(
  const Pdmp& pdmp, const State& initialState) {

  for (const auto& observer : this->observers_) {
    observer->notifyProcessBegins(pdmp, initialState);
  }
}

template<class Pdmp, class State, class RunningPolicy>
void PdmpRunner<Pdmp, State, RunningPolicy>::signalIterationResult(
  const IterationResult<State>& iterationResult) {

  for (const auto& observer : this->observers_) {
    observer->notifyIterationResult(iterationResult);
  }
}

template<class Pdmp, class State, class RunningPolicy>
void PdmpRunner<Pdmp, State, RunningPolicy>::signalProcessEnded() {
  for (const auto& observer : this->observers_) {
    observer->notifyProcessEnded();
  }
}

}
}
