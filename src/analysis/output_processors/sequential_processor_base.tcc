#pragma once

namespace pdmp {
namespace analysis {

template<class Pdmp, class State>
void SequentialProcessorBase<Pdmp,State>::notifyProcessBegins(
  const Pdmp& pdmp, const State& initialState) {

  this->observedPdmp_ = &pdmp;
  this->lastResult_ = IterationResult<State>(initialState, 0.0f);
}

template<class Pdmp, class State>
void SequentialProcessorBase<Pdmp, State>::notifyIterationResult(
  const IterationResult<State>& iterationResult) {

  this->processTwoSequentialResults(this->lastResult_, iterationResult);
  this->lastResult_ = iterationResult;
}

}
}
