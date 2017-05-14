#pragma once

namespace pdmp {
namespace analysis {

template<class PdmpRunner, class Pdmp, class State>
void FixedIterationsCountRunner::run(
  PdmpRunner& pdmpRunnerHost,
  Pdmp& pdmp,
  const State& initialState,
  long numberOfIterations) {

  static_cast<PdmpRunner*>(this)->signalProcessBegins(pdmp, initialState);
  State lastState = initialState;
  for (int i = 0; i < numberOfIterations; i++) {
    auto result = pdmp.simulateOneIteration(lastState);
    lastState = result.state;
    static_cast<PdmpRunner*>(this)->signalIterationResult(result);
  }
  static_cast<PdmpRunner*>(this)->signalProcessEnded();
}

}
}
