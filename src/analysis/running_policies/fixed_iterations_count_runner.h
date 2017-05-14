#pragma once

namespace pdmp {
namespace analysis {

/**
 * The simplest possible runner, simply running a given pdmp for
 * a requested number of iterations.
 */
class FixedIterationsCountRunner {

 protected:

  /**
   * The running policy runs the given pdmp for a specified number of
   * iterations. Should be called by the PdmpRunner host class.
   *
   * @param pdmp
   *   The Pdmp that we want to run.
   * @param initialState
   *   The state used to start our process.
   * @param numberOfIterations
   *   Runs the given process for the specified number of iterations.
   */
  template<class PdmpRunner, class Pdmp, class State>
  void run(
    PdmpRunner& pdmpRunnerHost,
    Pdmp& pdmp,
    const State& initialState,
    long numberOfIterations);
};

}
}

#include "fixed_iterations_count_runner.tcc"
