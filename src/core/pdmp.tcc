#pragma once

#include <limits>

namespace pdmp {

template<class State>
IterationResult<State>::IterationResult(
  const State& newState,
  const RealType& iterationTime)
    : state(newState),
      iterationTime(iterationTime) {
}

template<class State>
bool operator==(
  const IterationResult<State>& lhs, const IterationResult<State>& rhs) {

  return lhs.state == rhs.state
         && std::abs(lhs.iterationTime - rhs.iterationTime)
            < std::numeric_limits<typename State::RealType>::min();
}

// Constructor for initialising base classes.
// Collects type information on the given tuples and delegates to the
// private constructor.
template<class PoissonProcess, class MarkovKernel, class Intensity, class Flow>
template<
  class PoissonProcessArgsTuple,
  class MarkovKernelArgsTuple,
  class IntensityArgsTuple>
Pdmp<PoissonProcess, MarkovKernel, Intensity, Flow>::Pdmp(
  PoissonProcessArgsTuple&& poissonProcessArgs,
  MarkovKernelArgsTuple&& markovKernelArgs,
  IntensityArgsTuple&& intensityArgs)
  : Pdmp(
    std::forward<PoissonProcessArgsTuple>(poissonProcessArgs),
    std::forward<MarkovKernelArgsTuple>(markovKernelArgs),
    std::forward<IntensityArgsTuple>(intensityArgs),
    helpers::TupleIndexSequences<PoissonProcessArgsTuple>::ids,
    helpers::TupleIndexSequences<MarkovKernelArgsTuple>::ids,
    helpers::TupleIndexSequences<IntensityArgsTuple>::ids) {
}

// The private constructor, unwrapping the given tuples and initialising and
// passing the arguments to base classes.
template<class PoissonProcess, class MarkovKernel, class Intensity, class Flow>
template<
  class PoissonProcessArgsTuple, size_t... PoissonProcessIds,
  class MarkovKernelArgsTuple, size_t... MarkovKernelIds,
  class IntensityArgsTuple, size_t... IntensityIds>
Pdmp<PoissonProcess, MarkovKernel, Intensity, Flow>::Pdmp(
  PoissonProcessArgsTuple&& poissonProcessArgs,
  MarkovKernelArgsTuple&& markovKernelArgs,
  IntensityArgsTuple&& intensityArgs,
  std::index_sequence<PoissonProcessIds...>,
  std::index_sequence<MarkovKernelIds...>,
  std::index_sequence<IntensityIds...>)
  : PoissonProcess(std::get<PoissonProcessIds>(poissonProcessArgs)...),
    MarkovKernel(std::get<MarkovKernelIds>(markovKernelArgs)...),
    Intensity(std::get<IntensityIds>(intensityArgs)...) {
}

template<class PoissonProcess, class MarkovKernel, class Intensity, class Flow>
template<class State>
IterationResult<State> Pdmp<PoissonProcess, MarkovKernel, Intensity, Flow>
  ::simulateOneIteration(const State& initialState) {

  auto iterationTime = getJumpTime(initialState, *this);
  State stateBeforeJump = advanceStateByFlow(initialState, iterationTime);
  State stateAfterJump = jump(stateBeforeJump, *this);
  return IterationResult<State>(stateAfterJump, iterationTime);
}

}
