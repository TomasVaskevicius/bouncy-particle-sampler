#pragma once

namespace bps {

template<typename T, int Dim>
BpsState<T, Dim>::BpsState(
      Eigen::Matrix<T, Dim, 1> location,
      Eigen::Matrix<T, Dim, 1> velocity,
      T lastEventTime)
  : location_(location),
    velocity_(velocity),
    lastEventTime_(lastEventTime) {
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BpsState<T, Dim>::getVectorRepresentedByState()
    const {

  return this->location_;
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BpsState<T, Dim>::getLocation() const {
  return this->location_;
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> BpsState<T, Dim>::getVelocity() const {
  return this->velocity_;
}

template<typename T, int Dim>
T BpsState<T, Dim>::getLastEventTime() const {
  return this->lastEventTime_;
}

}
