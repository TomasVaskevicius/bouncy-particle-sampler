#pragma once

#include <cmath>
#include <limits>
#include <memory>


namespace pdmp {
namespace dependencies_graph {

namespace {

// Initially, we will initialize lastEvents_ array to point to this element.
const auto dummyEvent = std::make_shared<PoissonProcessEvent>(
  -1, std::make_shared<PoissonProcessResult<>>(0.0f));

}

PoissonProcessResultBase::PoissonProcessResultBase(const double& time)
  : time(time) {
}

template<class Lambda>
PoissonProcessResult<Lambda>::PoissonProcessResult(
  const double& time, const Lambda& shouldAccept)
  : PoissonProcessResultBase(time), shouldAccept_(shouldAccept) {
}

template<class Lambda>
bool PoissonProcessResult<Lambda>::shouldAccept() {
  return this->shouldAccept_();
}

template<class Lambda>
std::shared_ptr<PoissonProcessResultBase> PoissonProcessResult<Lambda>
  ::cloneAndAddTime(const double& timeToAdd) {

  return std::make_shared<PoissonProcessResult<decltype(this->shouldAccept_)>>(
    this->time + timeToAdd, this->shouldAccept_);
}

template<class Lambda>
std::shared_ptr<PoissonProcessResultBase> wrapPoissonProcessResult(
  double time, const Lambda& lambda) {

  std::shared_ptr<PoissonProcessResultBase> result = std::make_shared<
    PoissonProcessResult<decltype(lambda)>>(time, lambda);
  return result;
}

PoissonProcessEvent::PoissonProcessEvent(
  const int& factorId,
  std::shared_ptr<PoissonProcessResultBase> result)
  : factorId(factorId),
    result(std::move(result)) {
}

bool operator<(
  const PoissonProcessEvent& lhs, const PoissonProcessEvent& rhs) {

  return lhs.result->time - rhs.result->time < 0.0f;
}

bool operator>(
  const PoissonProcessEvent& lhs, const PoissonProcessEvent& rhs) {

  return rhs < lhs;
}

bool operator<(
  const std::shared_ptr<PoissonProcessEvent>& lhs,
  const std::shared_ptr<PoissonProcessEvent>& rhs) {

  return *lhs < *rhs;
}

bool operator>(
  const std::shared_ptr<PoissonProcessEvent>& lhs,
  const std::shared_ptr<PoissonProcessEvent>& rhs) {

  return *lhs > *rhs;
}

template<class DependenciesGraph, template<class> class EventScheduler>
PoissonProcess<DependenciesGraph, EventScheduler>::PoissonProcess(
  std::shared_ptr<DependenciesGraph> dependenciesGraph)
  : dependenciesGraph_(dependenciesGraph),
    latestEvents_(dependenciesGraph->factorNodes.size()) {

  factorsToResimulate_.clear();
  for (int i = 0; i < dependenciesGraph_->factorNodes.size(); i++) {
    factorsToResimulate_.push_back(i);
    latestEvents_[i] = dummyEvent;
  }
}

template<class DependenciesGraph, template<class> class EventScheduler>
template<class State, class HostClass>
auto PoissonProcess<DependenciesGraph, EventScheduler>::getJumpTime(
  const State& state, const HostClass& hostClass) {

  this->resimulateExpiredFactors(state);
  // Find the first valid event that is not rejected.
  while (true) {
    SharedPtrToEvent event = eventScheduler_.top();
    eventScheduler_.pop();
    if (!event->isValid) {
      continue;
    }
    if (!event->result->shouldAccept()) {
      // Rejected due to thinning step.
      double timeDifference = event->result->time - this->currentTime_;
      State rejectedState = hostClass.advanceStateByFlow(state, timeDifference);
      this->resimulateEventForFactor(
        rejectedState, event->factorId, event->result->time);
      continue;
    }
    // Found an event that is valid and not rejected.
    this->lastFactorId_ = event->factorId;
    this->factorsToResimulate_ =
      this->dependenciesGraph_->getFactorDependencies(this->lastFactorId_);
    auto returnTime = event->result->time - this->currentTime_;
    this->currentTime_ = event->result->time;
    return returnTime;
  }
}

template<class DependenciesGraph, template<class> class EventScheduler>
int PoissonProcess<DependenciesGraph, EventScheduler>::getLastFactorId() const {
  return this->lastFactorId_;
}

template<class DependenciesGraph, template<class> class EventScheduler>
template<class State>
void PoissonProcess<DependenciesGraph, EventScheduler>
  ::resimulateEventForFactor(
     const State& state, const int& factorId, const double& startingTime) {

  this->latestEvents_[factorId]->isValid = false;
  auto result = this->dependenciesGraph_->factorNodes.at(factorId)
    ->getPoissonProcessResult(state);
  auto resultWithUpdatedTime = result->cloneAndAddTime(startingTime);
  SharedPtrToEvent newEvent = std::make_shared<PoissonProcessEvent>(
    factorId, resultWithUpdatedTime);
  this->eventScheduler_.push(newEvent);
  this->latestEvents_[factorId] = newEvent;
}

template<class DependenciesGraph, template<class> class EventScheduler>
template<class State>
void PoissonProcess<DependenciesGraph, EventScheduler>
  ::resimulateExpiredFactors(const State& state) {

  bool lastFactorResimulated = false;
  for (const int& factorId : this->factorsToResimulate_) {
    this->resimulateEventForFactor(state, factorId, this->currentTime_);
    if (factorId == this->lastFactorId_) {
      lastFactorResimulated = true;
    }
  }
  if (!lastFactorResimulated) {
    this->resimulateEventForFactor(
      state, this->lastFactorId_, this->currentTime_);
  }
}

}
}
