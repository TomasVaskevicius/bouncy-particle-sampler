#pragma once

#include "bouncy_particle_sampler/bps_state.h"
#include "core/mcmc.h"

#include <Eigen/Core>
#include <memory>

namespace bps {

/**
 * Helper methods for BPS algorithms.
 */
template<typename FloatingPointType, int Dimensionality>
class BpsUtils {

 public:

  /**
   * Creates a new BpsState object with required position and velocity
   * and gives ownership of the new object to the caller.
   *
   * Returned object is wrapped as a base class McmcState.
   */
  static std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      createBpsMcmcState(
          Eigen::Matrix<FloatingPointType, Dimensionality, 1> position,
          Eigen::Matrix<FloatingPointType, Dimensionality, 1> velocity,
          FloatingPointType eventTime);

  /**
   * Creates a new BpsState object. Same as creteBpsMcmcState except that
   * the unique_ptr type is different here.
   */
  static std::unique_ptr<BpsState<FloatingPointType, Dimensionality>>
      createBpsState(
          Eigen::Matrix<FloatingPointType, Dimensionality, 1> position,
          Eigen::Matrix<FloatingPointType, Dimensionality, 1> velocity,
          FloatingPointType eventTime);

};

}

#include "bouncy_particle_sampler/bps_utils.tcc"
