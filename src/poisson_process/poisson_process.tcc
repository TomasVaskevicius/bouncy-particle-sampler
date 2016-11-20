#pragma once

namespace bps {

template<typename T>
PoissonProcess<T>::PoissonProcess()
  : lastJumpTime_(0),
    rate1ExponentialDistributionGenerator_(
        std::exponential_distribution<T>((T) 1.0)) {
}

template<typename T>
void PoissonProcess<T>::reset() {
  setTime(0);
}


template<typename T>
void PoissonProcess<T>::setTime(T time) {
  this->lastJumpTime_ = time;
}

template<typename T>
T PoissonProcess<T>::generateExponentialRandomVariable(T rate) {
  T rate1ExponentialRandomVariable = rate1ExponentialDistributionGenerator_(
      this->rng_);
  return rate1ExponentialRandomVariable / rate;
}

}
