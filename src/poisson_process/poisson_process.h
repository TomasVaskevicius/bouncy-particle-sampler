#pragma once

#include "random_number_sampler.h"

#include <random>

namespace bps {

/**
 * A base class for all poisson point process implementations.
 * Each Poisson process derived from this class is assumed to be defined on
 * [0, inf) and start at time 0. The process is undefined at negative times.
 */
template<typename FloatingPointType>
class PoissonProcess
    : public RandomNumberSampler<FloatingPointType, 1> {

 public:

  PoissonProcess();

  ~PoissonProcess() {}

  /**
   * Forgets the current number of jumps and restarts the processi from time 0.
   * Equivalent to calling setTime(0).
   */
  virtual void reset();

  /**
   * Resets the process time to the specified value.
   * setTime(0) is equivalent to reset().
   * The method is undefined for negative time values.
   *
   * @param time
   *   The time from which the Poisson process should be continues.
   *   That is, setting time to t is equivalent to having the last
   *   jump at time t.
   */
  virtual void setTime(FloatingPointType time);

 protected:

  // Returns an exponential random variable with given rate parameter.
  FloatingPointType generateExponentialRandomVariable(FloatingPointType rate);

  std::exponential_distribution<FloatingPointType>
      rate1ExponentialDistributionGenerator_;

  FloatingPointType lastJumpTime_;

};

}

#include "poisson_process/poisson_process.tcc"
