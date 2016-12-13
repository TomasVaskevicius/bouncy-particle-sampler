#pragma once

#include "core/mcmc.h"

#include <Eigen/Core>
#include <functional>
#include <vector>

namespace bps {

/**
 * Expectation estimators for BPS algorithm output.
 */
template<typename FloatingPointType, int Dimensionality>
class BpsExpectationEstimators {

 public:

  /**
   * Expectation estimator by numerically integrating the path
   * of BPS algorithm output.
   *
   * @param samples
   *   Output of the BPS algorithm.
   * @param function
   *   The function which expectation with respect to the invariant distribution
   *   of the BPS algorithm we want to calculate.
   * @return
   *   The vector (of size equal to number of samples - 1) of expectation
   *   estimates. Estimate i is made by considering the first i-1 piecewise
   *   segments of the samples.
   */
  static std::vector<FloatingPointType> numericalIntegrationEstimator(
      const std::vector<std::shared_ptr<
                        McmcState<FloatingPointType, Dimensionality>>>&
      samples,
      const std::function<FloatingPointType(
                          Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>&
      function);

};

}

#include "bouncy_particle_sampler/estimators/expectation_estimators.tcc"
