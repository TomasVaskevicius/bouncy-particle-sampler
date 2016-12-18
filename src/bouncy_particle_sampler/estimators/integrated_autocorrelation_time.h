#pragma once

#include "bouncy_particle_sampler/basic_bps.h"
#include "core/mcmc.h"

#include <functional>
#include <memory>
#include <vector>

namespace bps {

/**
 * We implement methods for computing IACT for output of BPS algorithms.
 */
template<typename FloatingPointType, int Dimensionality>
class IntegratedAutocorrelationTime {

 public:

  /**
   * Calculates the IACT.
   *
   * @param samples
   *   A vector of samples generated by the BPS algorithm.
   * @param function
   *   When calculating means, we will calculate the expectation of this
   *   function with respect to the invariant distribution which is the target
   *   of bps samples.
   * @param numberOfBatches
   *   The number of batches used to estimate the Markov Chains CLT variance.
   */
  static FloatingPointType calculateIntegratedAutocorrelationTime(
      const typename Mcmc<FloatingPointType, Dimensionality>::SampleOutput&
        samples,
      const typename Mcmc<FloatingPointType, Dimensionality>
        ::RealFunctionOnSamples& function,
      const int numberOfBatches = 100);

};

}

#include "bouncy_particle_sampler/estimators/integrated_autocorrelation_time.tcc"
