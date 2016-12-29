#pragma once

#include "bouncy_particle_sampler/bps_utils.h"
#include "bouncy_particle_sampler/newtonian_bounce_operator.h"
#include "core/mcmc.h"
#include "poisson_process/homogeneous_poisson_process.h"
#include "poisson_process/nhpp_by_approximating_quantile.h"

#include <algorithm>
#include <random>

// Todo:
//
// 1) Refactor the flow function (linear flow in this case).
// 2) Refactor the Poisson Process factory.

namespace bps {

template<typename T, int Dim>
BasicBps<T, Dim>::BasicBps(
    const T& refreshRate,
    const EnergyGradient& energyGradient)
  : refreshRate_(refreshRate),
    energyGradient_(energyGradient),
    bounceOperator_(std::move(std::unique_ptr<BounceOperator<T, Dim>>(
        new NewtonianBounceOperator<T, Dim>()))) {

  this->lastState_ = this->getInitialState();
}

template<typename T, int Dim>
std::unique_ptr<McmcState<T, Dim>> BasicBps<T, Dim>::generateNextState() const {
  auto lastState = static_cast<BpsState<T, Dim>*>(this->lastState_.get());

  HomogeneousPoissonProcess<T> refreshmentPoissonProcess(this->refreshRate_);
  auto bouncePoissonProcess = this->generatePoissonProcessForState(
      *lastState);

  T lastEventTime = lastState->getLastEventTime();
  T refreshmentInterarrivalTime = refreshmentPoissonProcess.getNextSample()(0);
  T bounceInterarrivalTime = bouncePoissonProcess->getNextSample()(0);

  if (refreshmentInterarrivalTime < bounceInterarrivalTime) {
    // Simulate refreshment event.
    auto newPosition = this->calculateNewPosition(
        *lastState, refreshmentInterarrivalTime);
    auto newVelocity = this->getRefreshedVelocity();
    T newEventTime = lastEventTime + refreshmentInterarrivalTime;
    return BpsUtils<T, Dim>::createBpsMcmcState(
        newPosition, newVelocity, newEventTime);
  } else {
    // Simulate bounce event.
    auto newPosition = this->calculateNewPosition(
        *lastState, bounceInterarrivalTime);
    T newEventTime = lastEventTime + bounceInterarrivalTime;
    BpsState<T, Dim> bpsState(
        newPosition, lastState->getVelocity(), newEventTime);
    return bounceOperator_->getStateAfterBounce(
        bpsState, this->energyGradient_);
  }
}

template<typename T, int Dim>
typename Mcmc<T, Dim>::SampleOutput BasicBps<T, Dim>
    ::getBatchOfMcmcStatesByTrajectoryLength(T requestedLength) {

  T generatedLength = (T) 0.0;
  std::vector<std::shared_ptr<McmcState<T, Dim>>> batch;
  batch.push_back(this->lastState_);
  std::shared_ptr<McmcState<T, Dim>> lastState = this->lastState_;
  while (generatedLength < requestedLength) {
    this->lastState_ = this->generateNextState();
    batch.push_back(this->lastState_);

    T timeDifference = BpsUtils<T, Dim>::getTimeDifferenceBetweenBpsMcmcStates(
        lastState, this->lastState_);
    generatedLength += timeDifference;

    lastState = this->lastState_;
  }

  return batch;
}

template<typename T, int Dim>
T BasicBps<T, Dim>::getRefreshRate() const {
  return this->refreshRate_;
}

template<typename T, int Dim>
void BasicBps<T, Dim>::setRefreshRate(T refreshRate) {
  this->refreshRate_ = refreshRate;
}

template<typename T, int Dim>
T BasicBps<T, Dim>::evaluateIntensityAtState(const BpsState<T, Dim>& state)
  const {

  auto position = state.getLocation();
  auto velocity = state.getVelocity();
  T intensity = velocity.dot(this->energyGradient_(position));
  return std::max((T) 0.0, intensity);
}

template<typename T, int Dim>
BounceOperator<T, Dim>* BasicBps<T, Dim>::getBounceOperator() const {
  return this->bounceOperator_.get();
}

template<typename T, int Dim>
std::function<Eigen::Matrix<T, Dim, 1>(Eigen::Matrix<T, Dim, 1>)>
  BasicBps<T, Dim>::getEnergyGradient() const {

  return this->energyGradient_;
}

template<typename T, int Dim>
std::unique_ptr<McmcState<T, Dim>> BasicBps<T, Dim>::getInitialState() const {
  Eigen::Matrix<T, Dim, 1> location = this->getRefreshedVelocity();
  Eigen::Matrix<T, Dim, 1> velocity = this->getRefreshedVelocity();

  return BpsUtils<T, Dim>::createBpsMcmcState(location, velocity, (T) 0.0);
}


template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BasicBps<T, Dim>::getRefreshedVelocity() const {
  std::normal_distribution<T> normal(0.0, 1.0);

  Eigen::Matrix<T, Dim, 1> refreshedVelocity;
  for (int i = 0; i < Dim; i++) {
    refreshedVelocity(i) = normal(const_cast<BasicBps<T, Dim>*>(this)->rng_);
  }

  return refreshedVelocity;
}

template<typename T, int Dim>
std::unique_ptr<PoissonProcess<T>> BasicBps<T, Dim>
    ::generatePoissonProcessForState(const BpsState<T, Dim>& state) const {

  auto startingPosition = state.getLocation();
  auto velocity = state.getVelocity();

  return std::unique_ptr<PoissonProcess<T>>(
      new NonHomogeneousPoissonProcessByApproximatingQuantile<T>(
          [this, state] (T t) -> T {
            auto positionAtTimeT = this->calculateNewPosition(state, t);
            auto intensity = state.getVelocity().dot(
                this->energyGradient_(positionAtTimeT));

            return std::max((T) 0.0, intensity);
          }));
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BasicBps<T, Dim>::calculateNewPosition(
    const BpsState<T, Dim>& state, T time) const {

  auto lastPosition = state.getLocation();
  auto velocity = state.getVelocity();
  return lastPosition + velocity * time;
}

}
