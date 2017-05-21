#pragma once

#include <array>
#include <memory>
#include <queue>
#include <vector>

namespace pdmp {
namespace dependencies_graph {

namespace {

// This lambda expression will be implicitly used by Poisson process simulation
// policies not based on thinning.
const auto accept = [] () -> bool { return true; };

}

struct PoissonProcessResultBase {
  PoissonProcessResultBase(const double& time);
  virtual ~PoissonProcessResultBase() = default;
  virtual bool shouldAccept() = 0;
  virtual std::shared_ptr<PoissonProcessResultBase> cloneAndAddTime(
    const double& timeToAdd) = 0;
  const double time;
};

/**
 * A data structure, which will be returned by individual Poisson process
 * factors.
 */
template<class Lambda = decltype(accept)>
class PoissonProcessResult : public PoissonProcessResultBase {
 public:
  PoissonProcessResult(const double& time, const Lambda& shouldAccept = accept);
  virtual bool shouldAccept() override final;
  virtual std::shared_ptr<PoissonProcessResultBase> cloneAndAddTime(
    const double& timeToAdd) override final;
 private:
  Lambda shouldAccept_;
};

/**
 * A helper for easily creating a PoissonProcessResultBase pointer.
 */
template<class Lambda = decltype(accept)>
std::shared_ptr<PoissonProcessResultBase> wrapPoissonProcessResult(
  double time, const Lambda& lambda = accept);


/**
 * Struct for inserting into the event queue.
 */
struct PoissonProcessEvent {
  PoissonProcessEvent(
    const int& factorId,
    std::shared_ptr<PoissonProcessResultBase> result);

  const int factorId;
  bool isValid = true;
  std::shared_ptr<PoissonProcessResultBase> result;
};

bool operator<(const PoissonProcessEvent& lhs, const PoissonProcessEvent& rhs);
bool operator>(const PoissonProcessEvent& lhs, const PoissonProcessEvent& rhs);
bool operator<(
  const std::shared_ptr<PoissonProcessEvent>& lhs,
  const std::shared_ptr<PoissonProcessEvent>& rhs);
bool operator>(
  const std::shared_ptr<PoissonProcessEvent>& lhs,
  const std::shared_ptr<PoissonProcessEvent>& rhs);

template<class EventType>
struct PriorityQueueEventScheduler
  : public std::priority_queue<
             EventType,
             std::vector<EventType>,
             std::greater<EventType>> {
};

/**
 * Poisson process policy based on the dependencies graph.
 */
template<
  class DependenciesGraph,
  template<class> class EventScheduler = PriorityQueueEventScheduler>
class PoissonProcess {

 public:

  using SharedPtrToEvent = std::shared_ptr<PoissonProcessEvent>;
  using FactorNodes = typename DependenciesGraph::FactorNodes;

  PoissonProcess(std::shared_ptr<DependenciesGraph> dependenciesGraph);

  template<class State, class HostClass>
  auto getJumpTime(const State& state, const HostClass& hostClass);

  int getLastFactorId() const;

 private:

  // Invalidates the last event of the given factor and simulates a new
  // event.
  template<class State>
  void resimulateEventForFactor(
    const State& state, const int& factorId, const double& startingTime);

  // Resimulates all Poisson processes with ids in factorsToResimulate_.
  // Set old events (from latestEvenets_ array) as invalid.
  template<class State>
  void resimulateExpiredFactors(const State& state);

  EventScheduler<SharedPtrToEvent> eventScheduler_;
  std::shared_ptr<DependenciesGraph> dependenciesGraph_;
  std::vector<int> factorsToResimulate_;
  std::vector<SharedPtrToEvent> latestEvents_;
  double currentTime_ = 0.0f;
  int lastFactorId_ = 0;

};

}
}

#include "poisson_process.tcc"
