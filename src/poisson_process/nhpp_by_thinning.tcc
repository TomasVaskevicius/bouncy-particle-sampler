#pragma once

namespace bps {

template<typename T>
NonHomogeneousPoissonProcessByThinning<T>
    ::NonHomogeneousPoissonProcessByThinning(
        std::function<T(T)> intensity,
        std::unique_ptr<PoissonProcess<T>> otherPoissonProcess,
        std::function<T(T)> otherIntensity)
  : NonHomogeneousPoissonProcess<T>(intensity),
    otherPoissonProcess_(std::move(otherPoissonProcess)),
    otherIntensity_(otherIntensity),
    uniformDistributionGenerator_(
        std::uniform_real_distribution<T>((T) 0.0, (T) 1.0)) {
}

template<typename T>
void NonHomogeneousPoissonProcessByThinning<T>::setTime(T time) {
  // If we want to reset this process to a particular time,
  // using the thinning implementation we need to reset the
  // other process to the same time as well.
  (this->otherPoissonProcess_)->setTime(time);
  this->lastJumpTime_ = time;
}

template<typename T>
Eigen::Matrix<T, 1, 1> NonHomogeneousPoissonProcessByThinning<T>
    ::getNextSample() {

  T proposal;
  do {
    proposal = otherPoissonProcess_->getNextSamples(1).at(0)(0);
  } while(uniformDistributionGenerator_(this->rng_)
          > this->intensity_(proposal) / this->otherIntensity_(proposal));
  this->lastJumpTime_ = proposal;

  // Wrap the next generated jump time into Eigen column vector.
  Eigen::Matrix<T, 1, 1> nextSample;
  nextSample << this->lastJumpTime_;

  return nextSample;
}

}
