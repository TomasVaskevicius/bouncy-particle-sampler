#pragma once

#include <type_traits>
#include <utility>

namespace pdmp {
namespace analysis {

template<class PdmpRunner, class Pdmp, class State>
void FixedIterationsCountRunner::run(
  PdmpRunner& pdmpRunnerHost,
  Pdmp& pdmp,
  State&& initialState,
  long numberOfIterations) {

  using State_t = std::decay_t<State>;
  static_cast<PdmpRunner*>(this)->signalProcessBegins(pdmp, initialState);
  IterationResult<State_t> lastResult(std::forward<State>(initialState), 0.0);
  for (int i = 0; i < numberOfIterations; i++) {
    lastResult = pdmp.simulateOneIteration(std::move(lastResult.state));
    static_cast<PdmpRunner*>(this)->signalIterationResult(lastResult);
  }
  static_cast<PdmpRunner*>(this)->signalProcessEnded();
}

}
}
