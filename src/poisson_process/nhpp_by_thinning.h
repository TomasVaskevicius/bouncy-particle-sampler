#pragma once

#include "poisson_process/non_homogeneous_poisson_process.h"

#include <Eigen/Core>
#include <memory>
#include <functional>

namespace bps {

/**
 * A thinning implementation for simulating non-homogeneous Poisson process.
 */
template<typename FloatingPointType>
class NonHomogeneousPoissonProcessByThinning
    : public NonHomogeneousPoissonProcess<FloatingPointType> {

 public:

  /**
   * Thinning implementation requires ability to sample another non-homogeneous
   * Poisson process which intensity is non smaller everywhere than the one we
   * want to sample from.
   *
   * @param intensity
   *   The intensity of the Poisson process we want to sample from.
   * @param otherPoissonProcess
   *   The Poisson process sampler with the intensity given in the next
   *   constructor argument.
   * @param otherIntensity
   *   The intensity of the Poisson process given in the first parameter.
   *
   */
  NonHomogeneousPoissonProcessByThinning(
      std::function<FloatingPointType(FloatingPointType)> intensity,
      std::unique_ptr<PoissonProcess<FloatingPointType>> otherPoissonProcess,
      std::function<FloatingPointType(FloatingPointType)> otherIntensity);


  void setTime(FloatingPointType time) override;

  Eigen::Matrix<FloatingPointType, 1, 1> getNextSample() override;

 private:

  const std::function<FloatingPointType(FloatingPointType)> otherIntensity_;

  std::unique_ptr<PoissonProcess<FloatingPointType>> otherPoissonProcess_;

  // Generates U[0,1) random variables.
  std::uniform_real_distribution<FloatingPointType>
      uniformDistributionGenerator_;

};

}

#include "poisson_process/nhpp_by_thinning.tcc"
