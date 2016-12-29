#pragma once

#include "bouncy_particle_sampler/basic_bps.h"

#include <Eigen/Dense>

/**
 * We implement energy gradient functions for Gaussian distributions in
 * this class.
 */
template<typename FloatingPointType, int Dimension>
class GaussianDistributions {

 public:

  typedef typename bps::BasicBps<FloatingPointType, Dimension>::EnergyGradient
          EnergyGradient;

  static EnergyGradient getGaussianDistributionEnergyGradient(
    const Eigen::Matrix<FloatingPointType, Dimension, 1>& mean,
    const Eigen::Matrix<FloatingPointType, Dimension, Dimension>&
      covarianceMatrix);

};

#include "gaussian.tcc"
