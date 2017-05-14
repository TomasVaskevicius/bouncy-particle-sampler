#pragma once

#include "analysis/pdmp_runner.h"

namespace pdmp {
namespace analysis {

template<class Pdmp, class State>
class SequentialProcessorBase : ObserverBase<Pdmp, State> {

 public:

  virtual ~SequentialProcessorBase() = default;

  virtual void notifyProcessBegins(
    const Pdmp& pdmp, const State& initialState) override final;

  virtual void notifyIterationResult(
    const IterationResult<State>& iterationResult) override final;

 protected:

  // Derived processors core functionality should be implemented here.
  // Should process the passed argument together with the
  virtual void processTwoSequentialResults(
    const IterationResult<State>& first,
    const IterationResult<State>& second) = 0;

  const Pdmp* observedPdmp_ = nullptr;
  IterationResult<State> lastResult_{State{}, 0.0f};
};

}
}

#include "sequential_processor_base.tcc"
