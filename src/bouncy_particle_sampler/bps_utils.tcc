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
    const std::vector<std::shared_ptr<McmcState<T, Dim>>>& samples) {

  T totalLength = (T) 0.0;
  for (int i = 1; i < samples.size(); i++) {
    totalLength += BpsUtils<T, Dim>::getTimeDifferenceBetweenBpsMcmcStates(
      samples[i-1], samples[i]);
  }
  return totalLength;
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

}
