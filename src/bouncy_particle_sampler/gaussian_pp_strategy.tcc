#pragma once

#include <chrono>
#include <cmath>

namespace bps {

template<typename T, int Dim>
PoissonProcessStrategyForGaussianTarget<T, Dim>
  ::PoissonProcessStrategyForGaussianTarget(
    const Eigen::Matrix<T, Dim, Dim>& covarianceMatrixInverse,
    const EnergyGradient& energyGradient)
  : BpsPoissonProcessStrategy<T, Dim>(energyGradient),
    covarianceMatrixInverse_(covarianceMatrixInverse) {

  int seed = std::chrono::system_clock::now().time_since_epoch().count();
  this->rng_ = std::default_random_engine(seed);
}

template<typename T, int Dim>
T PoissonProcessStrategyForGaussianTarget<T, Dim>::getNextEventTime(
  const BpsState<T, Dim>& bpsState) const {

  auto x = bpsState.getLocation();
  auto v = bpsState.getVelocity();

  // Calculate <x, v> with respect to inverted covariance matrix.
  T innerProductXandV = x.transpose() * this->covarianceMatrixInverse_ * v;

  // Calculate ||v||^2 with respect to inverted covariance matrix.
  T vNormSquared = v.transpose() * this->covarianceMatrixInverse_ * v;

  // Generate U ~ Unif[0,1]
  T U = this->generateUniformZeroOneRandomVariable();

  if (innerProductXandV <= 0.0) {
    return ((-1) * innerProductXandV + sqrt((-1) * vNormSquared * log(U)))
            / vNormSquared;
  } else {
    return ((-1) * innerProductXandV
            + sqrt(innerProductXandV * innerProductXandV
                   - vNormSquared * log(U))) / vNormSquared;
  }
}

template<typename T, int Dim>
T PoissonProcessStrategyForGaussianTarget<T, Dim>
  ::generateUniformZeroOneRandomVariable() const {

  std::uniform_real_distribution<T> unif((T) 0.0, (T) 1.0);
  return unif(this->rng_);
}

}
