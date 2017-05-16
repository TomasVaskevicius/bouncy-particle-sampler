#pragma once

#include "core/policies/linear_flow.h"

#include <tuple>
#include <utility>

namespace pdmp {

namespace helpers {
/**
 * A helper for unwrapping tuples for base (policy) class initialization.
 * This structs holds an index list for associated tuple.
 */
template<class Tuple>
struct TupleIndexSequences {
  static constexpr auto ids =
    std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{};
};

}

/**
 * A data structure for holding relevant information of each simulated
 * iteration. That is, it holds the state of the process just before the
 * jump happens, the state of the process after the jump, the factor
 * of Poisson process which fired and the time returned by the Poisson
 * process simulation.
 *
 * Constraints on template parameters:
 * 1) State should expose its floting point type via State::RealType.
 */
template<class State>
struct IterationResult {
  using RealType = typename State::RealType;
  IterationResult(const State& newState, const RealType& iterationTime);
  State state;
  RealType iterationTime;
};

template<class State>
bool operator==(
  const IterationResult<State>& lhs, const IterationResult<State>& rhs);

/**
 * A class for simulation of piecewise-deterministic Markov processes.
 * We use the policy-based design pattern (i.e., each template argument
 * implements a particular policy, while this class combines their behaviour
 * in a way which simulates a piecewise-deterministic Markov process).
 *
 * Constraints:
 * 1) State should expose its floating point type by State::RealType.
 * 2) Flow, JumpKernel, PoissonProcess and Intensity should all be compatible
 *    with the given state space.
 * 3) Flow should implement a method
 *    State advanceStateByFlow(State, State::RealType).
 * 4) MarkovKernel should implement a method
 *    State jump(State).
 * 5) PoissonProcess should implement a method
 *    State::RealType getJumpTime(State).
 */
template<
  class PoissonProcess,
  class MarkovKernel,
  class Flow = LinearFlow>
class Pdmp :
  public PoissonProcess,
  public MarkovKernel,
  public Flow {

 public:

  using PoissonProcess::getJumpTime;
  using MarkovKernel::jump;
  using Flow::advanceStateByFlow;

  Pdmp() = default;

  /**
   * A constructor, which takes three tuples of parameters, each of which
   * is unwrapped and passed to the base class constructors.
   */
  template<
    class PoissonProcessArgsTuple,
    class MarkovKernelArgsTuple>
  Pdmp(
    PoissonProcessArgsTuple&& poissonProcessArgs,
    MarkovKernelArgsTuple&& markovKernelArgs);

  /**
   * Simulates one complete iteration of the process. That is, starting from
   * the given state, evolves the process according to the deterministic flow
   * function until a spontaneous Poisson process event occurs, after which
   * a random jump is made.
   *
   * @initialState
   *   Initial state of the process.
   * @return
   *   An iteration result data structure, encoding all the relevant
   *   information.
   */
  template<class State>
  IterationResult<State> simulateOneIteration(const State& initialState);

 private:

  // A helper constructor with tuple index sequence types information.
  template<
    class PoissonProcessArgsTuple, size_t... PoissonProcessIndexSeq,
    class MarkovKernelArgsTuple, size_t... MarkovKernelIndexSeq>
  Pdmp(
    PoissonProcessArgsTuple&& poissonProcessArgs,
    MarkovKernelArgsTuple&& markovKernelArgs,
    std::index_sequence<PoissonProcessIndexSeq...>,
    std::index_sequence<MarkovKernelIndexSeq...>);

};

}

#include "pdmp.tcc"
