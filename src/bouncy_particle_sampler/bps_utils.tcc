#pragma once

#include "bouncy_particle_sampler/bps_state.h"

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

}
