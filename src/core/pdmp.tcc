#pragma once

#include <type_traits>
#include <limits>

namespace pdmp {

template<class State>
IterationResult<State>::IterationResult(
  const State& newState,
  RealType iterationTime)
    : state(newState),
      iterationTime(iterationTime) {
}

template<class State>
IterationResult<State>::IterationResult(
  State&& newState,
  RealType iterationTime)
    : state(std::move(newState)),
      iterationTime(iterationTime) {
}

template<class State>
bool operator==(
  const IterationResult<State>& lhs, const IterationResult<State>& rhs) {

  return lhs.state == rhs.state
         && std::abs(lhs.iterationTime - rhs.iterationTime) < 1e-7;
}

// Constructor for initialising base classes.
// Collects type information on the given tuples and delegates to the
// private constructor.
template<class PoissonProcess, class MarkovKernel, class Flow>
template<class PoissonProcessArgsTuple, class MarkovKernelArgsTuple>
Pdmp<PoissonProcess, MarkovKernel, Flow>::Pdmp(
  PoissonProcessArgsTuple&& poissonProcessArgs,
  MarkovKernelArgsTuple&& markovKernelArgs)
  : Pdmp(
    std::forward<PoissonProcessArgsTuple>(poissonProcessArgs),
    std::forward<MarkovKernelArgsTuple>(markovKernelArgs),
    helpers::TupleIndexSequences<PoissonProcessArgsTuple>::ids,
    helpers::TupleIndexSequences<MarkovKernelArgsTuple>::ids) {
}

// The private constructor, unwrapping the given tuples and initialising and
// passing the arguments to base classes.
template<class PoissonProcess, class MarkovKernel, class Flow>
template<
  class PoissonProcessArgsTuple, size_t... PoissonProcessIds,
  class MarkovKernelArgsTuple, size_t... MarkovKernelIds>
Pdmp<PoissonProcess, MarkovKernel, Flow>::Pdmp(
  PoissonProcessArgsTuple&& poissonProcessArgs,
  MarkovKernelArgsTuple&& markovKernelArgs,
  std::index_sequence<PoissonProcessIds...>,
  std::index_sequence<MarkovKernelIds...>)
  : PoissonProcess(std::get<PoissonProcessIds>(poissonProcessArgs)...),
    MarkovKernel(std::get<MarkovKernelIds>(markovKernelArgs)...) {
}

template<class PoissonProcess, class MarkovKernel, class Flow>
template<class State>
IterationResult<std::decay_t<State>> Pdmp<PoissonProcess, MarkovKernel, Flow>
  ::simulateOneIteration(State&& initialState) {

  using State_t = std::decay_t<State>;
  auto iterationTime = getJumpTime(initialState, *this);
  State_t stateBeforeJump = advanceStateByFlow(
    std::forward<State>(initialState), iterationTime);
  State_t stateAfterJump = jump(std::move(stateBeforeJump), *this);
  return IterationResult<State_t>(
    std::move(stateAfterJump), iterationTime);
}

}
