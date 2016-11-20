#pragma once

namespace bps {

template<typename T>
HomogeneousPoissonProcess<T>::HomogeneousPoissonProcess(T rate)
  : rate_(rate) {
}

template<typename T>
Eigen::Matrix<T, 1, 1> HomogeneousPoissonProcess<T>
    ::getNextSample() {

  T interarrivalTime = this->generateExponentialRandomVariable(this->rate_);
  this->lastJumpTime_ += interarrivalTime;

  // Wrap the next sample jump time into Eigen column vector.
  Eigen::Matrix<T, 1, 1> nextSample;
  nextSample << this->lastJumpTime_;

  return nextSample;
}

}
