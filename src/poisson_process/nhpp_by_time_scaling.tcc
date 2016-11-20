#pragma once

#include <stdexcept>

namespace bps {

template<typename T>
NonHomogeneousPoissonProcessByTimeScaling<T>
    ::NonHomogeneousPoissonProcessByTimeScaling(
        std::function<T(T)> integratedIntensityQuantileFunction)
  : NonHomogeneousPoissonProcessByTimeScaling(
        integratedIntensityQuantileFunction,
        [] (T) -> T {
          // Since the actual intensity function was not provided
          // we just pass a dummy intensity function.
          return 0.0;
        }) {
}

template<typename T>
void NonHomogeneousPoissonProcessByTimeScaling<T>::setTime(T time) {
  if (time == 0) {
    (this->rate1PoissonProcess_)->setTime(0);
    this->lastJumpTime_ = 0;
  } else {
    throw std::runtime_error(
        "Setting non-zero time to Poisson process implemented using time "
	"scaling is not supported.");
  }
}

template<typename T>
Eigen::Matrix<T, 1, 1> NonHomogeneousPoissonProcessByTimeScaling<T>
    ::getNextSample() {

    T nextUnscaledJumpTime = (this->rate1PoissonProcess_)->getNextSample()(0);
    this->lastJumpTime_ = this->integratedIntensityQuantileFunction_(
        nextUnscaledJumpTime);

    // Wrap the next generated jump time into Eigen column vector.
    Eigen::Matrix<T, 1, 1> nextSample;
    nextSample << this->lastJumpTime_;

    return nextSample;
}

// Protected constructor.
template<typename T>
NonHomogeneousPoissonProcessByTimeScaling<T>
    ::NonHomogeneousPoissonProcessByTimeScaling(
        std::function<T(T)> integratedIntensityQuantileFunction,
        std::function<T(T)> intensity)
  : NonHomogeneousPoissonProcess<T>(intensity),
    integratedIntensityQuantileFunction_(integratedIntensityQuantileFunction),
    rate1PoissonProcess_(
        std::unique_ptr<HomogeneousPoissonProcess<T>>(
            new HomogeneousPoissonProcess<T>((T) 1.0))) {
}

}
