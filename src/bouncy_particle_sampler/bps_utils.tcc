#pragma once

#include "bouncy_particle_sampler/bps_state.h"
#include "gsl_wrappers/integration_wrappers.h"

namespace bps {

template<typename T, int Dim>
std::unique_ptr<McmcState<T, Dim>> BpsUtils<T, Dim>::createBpsMcmcState(
    Eigen::Matrix<T, Dim, 1> position,
    Eigen::Matrix<T, Dim, 1> velocity,
    T eventTime) {

  return std::unique_ptr<bps::McmcState<T, Dim>>(
      new bps::BpsState<T, Dim>(position, velocity, eventTime));
}

template<typename T, int Dim>
std::unique_ptr<BpsState<T, Dim>> BpsUtils<T, Dim>::createBpsState(
    Eigen::Matrix<T, Dim, 1> position,
    Eigen::Matrix<T, Dim, 1> velocity,
    T eventTime) {

  return std::unique_ptr<bps::BpsState<T, Dim>>(
      new bps::BpsState<T, Dim>(position, velocity, eventTime));
}

template<typename T, int Dim>
T BpsUtils<T, Dim>::getTimeDifferenceBetweenBpsMcmcStates(
    std::shared_ptr<McmcState<T, Dim>> oldState,
    std::shared_ptr<McmcState<T, Dim>> newState) {

  auto oldTime = static_cast<bps::BpsState<T, Dim>*>(
      oldState.get())->getLastEventTime();
  auto newTime = static_cast<bps::BpsState<T, Dim>*>(
      newState.get())->getLastEventTime();
  return newTime - oldTime;
}

template<typename T, int Dim>
T BpsUtils<T, Dim>::getTotalPathLength(
    const typename Mcmc<T, Dim>::SampleOutput& samples) {

  T initialTime = BpsUtils<T, Dim>::getEventTimeFromMcmcState(
      samples[0]);
  T lastTime = BpsUtils<T, Dim>::getEventTimeFromMcmcState(
      samples[samples.size()-1]);

  return lastTime - initialTime;
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BpsUtils<T, Dim>::getLocationFromMcmcState(
    const std::shared_ptr<bps::McmcState<T, Dim>>& state) {

  return static_cast<bps::BpsState<T, Dim>*>(state.get())->getLocation();
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BpsUtils<T, Dim>::getVelocityFromMcmcState(
    const std::shared_ptr<bps::McmcState<T, Dim>>& state) {

  return static_cast<bps::BpsState<T, Dim>*>(state.get())->getVelocity();
}

template<typename T, int Dim>
T BpsUtils<T, Dim>::getEventTimeFromMcmcState(
    const std::shared_ptr<bps::McmcState<T, Dim>>& state) {

  return static_cast<bps::BpsState<T, Dim>*>(state.get())->getLastEventTime();
}

template<typename T, int Dim>
T BpsUtils<T, Dim>::integrateAlongPiecewiseLinearSegment(
    const std::shared_ptr<McmcState<T, Dim>>& leftEndpoint,
    const std::shared_ptr<McmcState<T, Dim>>& rightEndpoint,
    const std::function<T(Eigen::Matrix<T, Dim, 1>)>& function) {

  auto location = BpsUtils<T, Dim>::getLocationFromMcmcState(leftEndpoint);
  auto velocity = BpsUtils<T, Dim>::getVelocityFromMcmcState(leftEndpoint);
  T timeDifference = BpsUtils<T, Dim>::getTimeDifferenceBetweenBpsMcmcStates(
      leftEndpoint, rightEndpoint);

  std::function<T(T)> integrand = [&] (T t) -> T {
    return function(location + velocity * t);
  };

  T integral = GslIntegrationWrappers<T>::integrate(
      integrand, (T) 0.0, timeDifference);

  return integral;
}

template<typename T, int Dim>
std::shared_ptr<bps::McmcState<T, Dim>> BpsUtils<T, Dim>
    ::advanceBpsStateByLinearFlow(
        const std::shared_ptr<bps::McmcState<T, Dim>>& state,
        const T& requiredLength) {

  auto location = bps::BpsUtils<T, Dim>::getLocationFromMcmcState(state);
  auto velocity = bps::BpsUtils<T, Dim>::getVelocityFromMcmcState(state);
  auto newLocation = location + velocity * requiredLength;
  T newTime = bps::BpsUtils<T, Dim>::getEventTimeFromMcmcState(state)
      + requiredLength;
  return bps::BpsUtils<T, Dim>::createBpsMcmcState(
      newLocation, velocity, newTime);
}

template<typename T, int Dim>
std::vector<Eigen::Matrix<T, Dim, 1>> BpsUtils<T, Dim>
    ::getEquallySpacedPointsFromBpsRun(
      const typename Mcmc<T, Dim>::SampleOutput& samples,
      const T& jumpLength) {

  std::vector<Eigen::Matrix<T, Dim, 1>> equallySpacedPoints;

  auto leftEndpoint = samples[0];
  auto rightEndpoint = samples[1];
  int nextPiecewiseSegmentRightIndex = 2;

  T remainderUntilNextPoint = 0;
  // This loop will break once we run out of bps samples.
  while (true) {
    if (remainderUntilNextPoint == 0) {
      equallySpacedPoints.push_back(leftEndpoint->getVectorRepresentedByState());
      remainderUntilNextPoint = jumpLength;
    }
    if (BpsUtils<T, Dim>::getTimeDifferenceBetweenBpsMcmcStates(
        leftEndpoint, rightEndpoint) >= remainderUntilNextPoint) {

      // We can get the next point from inbetween the two
      // current bps states (leftEndpoint and rightEndpoint).
      leftEndpoint = BpsUtils<T, Dim>::advanceBpsStateByLinearFlow(
          leftEndpoint, remainderUntilNextPoint);
      remainderUntilNextPoint = 0.0;
    } else if(nextPiecewiseSegmentRightIndex < samples.size()) {
      // We need to take the next bps segment and such segment exists.
      remainderUntilNextPoint -= BpsUtils<T, Dim>
          ::getTimeDifferenceBetweenBpsMcmcStates(leftEndpoint, rightEndpoint);
      leftEndpoint = rightEndpoint;
      rightEndpoint = samples[nextPiecewiseSegmentRightIndex];
      nextPiecewiseSegmentRightIndex++;
    } else {
      // We need to take the next bps segment, but we ran out of samples.
      break;
    }
  }

  return equallySpacedPoints;
}

}
