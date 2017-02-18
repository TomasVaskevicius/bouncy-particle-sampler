#pragma once

#include "bouncy_particle_sampler/pp_strategy.h"

#include <functional>
#include <memory>
#include <random>

#include <Eigen/Core>

namespace bps {

/**
 * This class implements efficient inhomogeneous Poisson process simulation for
 * Gaussian target.
 */
template<typename FloatingPointType, int Dimensionality>
class PoissonProcessStrategyForGaussianTarget
  : public BpsPoissonProcessStrategy<FloatingPointType, Dimensionality> {

  typedef std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
                        Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>
          EnergyGradient;
 public:

  /**
   * The constructor needs to take the inverse of the covariance matrix of
   * the target multivariate Gaussian distribution in addition to the
   * energy function.
   */
  PoissonProcessStrategyForGaussianTarget(
    const Eigen::Matrix<FloatingPointType, Dimensionality, Dimensionality>&
      covarianceMatrixInverse,
    const EnergyGradient& energyGradient);

  FloatingPointType getNextEventTime(
    const BpsState<FloatingPointType, Dimensionality>& bpsState) const override;

 private:

  FloatingPointType generateUniformZeroOneRandomVariable() const;

  const Eigen::Matrix<FloatingPointType, Dimensionality, Dimensionality>
    covarianceMatrixInverse_;

  mutable std::default_random_engine rng_;

};

}

#include "gaussian_pp_strategy.tcc"
