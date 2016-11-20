#pragma once

#include "poisson_process/homogeneous_poisson_process.h"
#include "poisson_process/non_homogeneous_poisson_process.h"

#include <Eigen/Core>
#include <memory>
#include <functional>

namespace bps {

/**
 * A non-homogeneous Poisson process implementation by simulating rate 1
 * homogeneous Poisson process and using appropriate time scale transformation.
 */
template<typename FloatingPointType>
class NonHomogeneousPoissonProcessByTimeScaling
    : public NonHomogeneousPoissonProcess<FloatingPointType> {

 public:

  /**
   * The time-scale transformation method only requires the knowledge of the
   * quantile function of the integrated intensity function. Note that in
   * particluar.
   *
   * @param integratedIntensityQuantileFunction
   *   The quantile function of the integrated intensity function of the
   *   non-homogeneous Poisson process that we want to simulate.
   */
  NonHomogeneousPoissonProcessByTimeScaling(
      std::function<FloatingPointType(FloatingPointType)>
          integratedIntensityQuantileFunction);



  /**
   * Cannot reset the time to non-zero time without knowledge of
   * inverse of the quantile.
   *
   * If time is not equal to zero, will throw an exception.
   */
  void setTime(FloatingPointType time) override;

  Eigen::Matrix<FloatingPointType, 1, 1> getNextSample() override;

 protected:

  // Additional constructor for subclasses that can take intensity
  // as a parameter as well.
  NonHomogeneousPoissonProcessByTimeScaling(
      std::function<FloatingPointType(FloatingPointType)>
          integratedIntensityQuantileFunction,
      std::function<FloatingPointType(FloatingPointType)>
          intensity);

 private:

  std::function<FloatingPointType(FloatingPointType)>
      integratedIntensityQuantileFunction_;

  // A homogeneous poisson process with rate 1. Used as a subroutine
  // for simulating our process.
  std::unique_ptr<HomogeneousPoissonProcess<FloatingPointType>>
      rate1PoissonProcess_;

};

}

#include "poisson_process/nhpp_by_time_scaling.tcc"
