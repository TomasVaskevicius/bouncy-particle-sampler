#pragma once

#include "poisson_process/poisson_process.h"

#include <Eigen/Core>
#include <functional>
#include <random>

namespace bps {

/**
 * A homogeneous Poisson point process.
 * Implemented via simulation of i.i.d exponential interarrival times.
 */
template<typename FloatingPointType>
class HomogeneousPoissonProcess
    : public PoissonProcess<FloatingPointType> {

 public:

  /*
   * A homogeneous Poisson process is parameterized by the rate parameter.
   *
   * @param rate
   *   The rate parameter of this Poisson process. Must be strictly positive.
   */
  HomogeneousPoissonProcess(FloatingPointType rate);

  Eigen::Matrix<FloatingPointType, 1, 1>
      getNextSample() override final;

 private:

  FloatingPointType rate_;

};

}

#include "poisson_process/homogeneous_poisson_process.tcc"
