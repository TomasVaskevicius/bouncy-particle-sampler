#pragma once

#include "poisson_process/poisson_process.h"

#include <Eigen/Core>
#include <functional>

namespace bps {

/**
 * A base class for non-homogeneous Poisson processes.
 */
template<typename FloatingPointType>
class NonHomogeneousPoissonProcess
    : public PoissonProcess<FloatingPointType> {

 public:

  /**
   * A non-homogeneous Poisson process is parameterized by intensity function.
   *
   * @param intensity
   *   The intensity function which determines the evolution of the
   *   non-homogeneous poisson process. Must be everywhere non-negative.
   */
  NonHomogeneousPoissonProcess(
      std::function<FloatingPointType(FloatingPointType)> intensity);

 protected:

  const std::function<FloatingPointType(FloatingPointType)> intensity_;

};

}

#include "poisson_process/non_homogeneous_poisson_process.tcc"
