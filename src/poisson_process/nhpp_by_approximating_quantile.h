#pragma once

#include "poisson_process/nhpp_by_time_scaling.h"

#include <Eigen/Core>
#include <functional>

namespace bps {

/**
 * A time scale transformation method implementation where the integrated
 * intensity quantile function is not known explicitly. It will only be
 * approximated using numerical methods and hence the resulting process will
 * contain some noise.
 */
template<typename FloatingPointType>
class NonHomogeneousPoissonProcessByApproximatingQuantile
    : public NonHomogeneousPoissonProcessByTimeScaling<FloatingPointType> {

 public:

  /**
   * We only need to know the intensity function. Integrated quantile function
   * will be estimated using numerical methods and passed to the constructor
   * of the parent class (implementing time scale transformation method).
   *
   * @param intensity
   *   The intensity function of the non homogeneous Poisson process we
   *   want to simulate.
   */
  NonHomogeneousPoissonProcessByApproximatingQuantile(
      std::function<FloatingPointType(FloatingPointType)> intensity);

  void reset() override;

 private:

  // Returns the integrated intensity quantile function which will
  // be passed to the parent class constructor.
  std::function<FloatingPointType(FloatingPointType)>
      generateIntegratedIntensityQuantile();

  FloatingPointType integratedIntensityAtLastJumpTime_;

};

}

#include "poisson_process/nhpp_by_approximating_quantile.tcc"
