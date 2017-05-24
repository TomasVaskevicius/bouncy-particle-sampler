#pragma once

#include <memory>
#include <vector>

#include "core/pdmp.h"

namespace pdmp {
namespace analysis {

/**
 * The base class for observers of a running Pdmp.
 * Type of Pdmp and the state space must be specified as template
 * parameters.
 * We notify the observer with the Pdmp process object that is being observed,
 * so that the observer that use const member methods to extract relevant
 * information.
 */
template<class Pdmp, class State>
struct ObserverBase {
  virtual ~ObserverBase() = default;

  virtual void notifyProcessBegins(
    const Pdmp& pdmp, const State& initialState);

  virtual void notifyIterationResult(
    const IterationResult<State>& iterationResult);

  virtual void notifyProcessEnded();
};

/**
 * A class for running any Pdmp.
 * Its running policy is responsible for deciding when to stop the process
 * and when to notify the observers.
 */
template<class Pdmp, class State, class RunningPolicy>
class PdmpRunner : public RunningPolicy {

 public:

  using ObserverPtr = ObserverBase<Pdmp, State>*;

  /**
   * Runs the given pdmp at the starting state according to the
   * runner policy. All args... are forwarded to the running policy.
   */
  template<class... Args>
  void run(Pdmp& pdmp, const State& initialState, Args&&... args);

  /**
   * Runs the given pdmp at the starting state according to the
   * runner policy. All args... are forwarded to the running policy.
   */
  template<class... Args>
  void run(Pdmp& pdmp, State&& initialState, Args&&... args);

  /**
   * Registers a given observer to the process.
   */
  void registerAnObserver(ObserverPtr observer);

 private:

  // Allow the RunningPolicy to notify observers.
  friend RunningPolicy;

  // Notified all observers that the process is about to start.
  void signalProcessBegins(const Pdmp& pdmp, const State& initialState);

  // Notifies all the observers with the given iteration result.
  void signalIterationResult(
    const IterationResult<State>& iterationResult);

  // Notifies all observers that the process has ended.
  void signalProcessEnded();

  std::vector<ObserverPtr> observers_;
};

}
}

#include "pdmp_runner.tcc"
