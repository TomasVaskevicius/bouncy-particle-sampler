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

  /**
   * Returns the event time difference between two BpsState objects.
   */
  static FloatingPointType getTimeDifferenceBetweenBpsMcmcStates(
      std::shared_ptr<McmcState<FloatingPointType, Dimensionality>> oldState,
      std::shared_ptr<McmcState<FloatingPointType, Dimensionality>> newState);

  /**
   * Returns the path length of the BPS algorithm output samples.
   */
  static FloatingPointType getTotalPathLength(
      const typename Mcmc<FloatingPointType, Dimensionality>::SampleOutput&
          samples);

  /**
   * Extracts the BPS particle location from McmcState pointer.
   */
  static Eigen::Matrix<FloatingPointType, Dimensionality, 1>
      getLocationFromMcmcState(const std::shared_ptr<bps::McmcState<
          FloatingPointType, Dimensionality>>& state);

  /**
   * Extracts the BPS particle velocity from McmcState pointer.
   */
  static Eigen::Matrix<FloatingPointType, Dimensionality, 1>
      getVelocityFromMcmcState(const std::shared_ptr<bps::McmcState<
          FloatingPointType, Dimensionality>>& state);

  /**
   * Extracts the BPS particle event time from McmcState pointer.
   */
  static FloatingPointType
      getEventTimeFromMcmcState(const std::shared_ptr<bps::McmcState<
          FloatingPointType, Dimensionality>>& state);

  /**
   * Integrates a function along a piecewise linear segment determined by
   * two BPS states.
   */
  static FloatingPointType integrateAlongPiecewiseLinearSegment(
      const std::shared_ptr<McmcState<FloatingPointType, Dimensionality>>&
          leftEndpoint,
      const std::shared_ptr<McmcState<FloatingPointType, Dimensionality>>&
          rightEndpoint,
      const std::function<FloatingPointType(
          Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>& function);

};

}

#include "bouncy_particle_sampler/bps_utils.tcc"
