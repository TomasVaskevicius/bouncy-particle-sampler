#pragma once

#include "core/mcmc.h"

#include <Eigen/Core>

namespace bps {

/**
 * Mcmc state for the Bouncy Particle Sampler.
 * State is represented by particle location and velocity.
 */
template<typename FloatingPointType, int Dimensionality>
class BpsState : public McmcState<FloatingPointType, Dimensionality> {

 public:

  /**
   * We initialize the state by the given location, velocity
   * and the time of last jump yielding the current state..
   */
  BpsState(
      Eigen::Matrix<FloatingPointType, Dimensionality, 1> location,
      Eigen::Matrix<FloatingPointType, Dimensionality, 1> velocity,
      FloatingPointType lastEventTime);

  Eigen::Matrix<FloatingPointType, Dimensionality, 1>
      getVectorRepresentedByState() const override final;


  /**
   * Returns the location of the particle represented by this state.
   */
  Eigen::Matrix<FloatingPointType, Dimensionality, 1> getLocation() const;

  /**
   * Returns the velocity of the particle represented by this state.
   */
  Eigen::Matrix<FloatingPointType, Dimensionality, 1> getVelocity() const;

  /**
   * Returns the event time which yielded this state.
   */
  FloatingPointType getLastEventTime() const;

 private:

  const Eigen::Matrix<FloatingPointType, Dimensionality, 1> location_;

  const Eigen::Matrix<FloatingPointType, Dimensionality, 1> velocity_;

  const FloatingPointType lastEventTime_;

};

}

#include "bps_state.tcc"
