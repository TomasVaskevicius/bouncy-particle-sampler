#pragma once

#include "core/mcmc.h"
#include "bouncy_particle_sampler/bounce_operator.h"
#include "bouncy_particle_sampler/bps_state.h"
#include "bouncy_particle_sampler/bps_flow.h"
#include "bouncy_particle_sampler/pp_strategy.h"
#include "poisson_process/poisson_process.h"

#include <memory>
#include <functional>
#include <Eigen/Core>

namespace bps {

/**
 * The Basic Bouncy Particle Sampler algorithm.
 *
 * Algorithm 1 in "The Bouncy Particle Sampler:
 * A Non-Reversible Rejection-Free Markov Chain Monte Carlo Method".
 * See https://arxiv.org/abs/1510.02451.
 */
template<typename FloatingPointType, int Dimensionality>
class BasicBps : public Mcmc<FloatingPointType, Dimensionality> {

 public:

  typedef std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
                        Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>
          EnergyGradient;

  /**
    * Right now the most basic implementation takes only refresh rate and
    * the energyGradient as a parameter.
    *
    * This constructor will default to using numerical strategy for generating
    * the Poisson process samples.
    */
  BasicBps(
    const FloatingPointType& refreshRate,
    const EnergyGradient& energyGradient);

  /**
   * This constructor allows to give a user defined Poisson process generation
   * strategy.
   */
  BasicBps(
    const FloatingPointType& refreshRate,
    const EnergyGradient& energyGradient,
    std::unique_ptr<BpsPoissonProcessStrategy<
      FloatingPointType, Dimensionality>> ppStrategy);

  std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      generateNextState() const override;

  /**
   * Returns a vector of samples such that the total trajectory length
   * is at least as long as the requested length.
   */
  typename Mcmc<FloatingPointType, Dimensionality>::SampleOutput
      getBatchOfMcmcStatesByTrajectoryLength(FloatingPointType requestedLength);

  /**
   * Returns the refresh rate.
   */
  FloatingPointType getRefreshRate() const;

  /**
   * Changes the refresh rate.
   */
  void setRefreshRate(FloatingPointType refreshRate);

  /**
   * Evaluates the Poisson process intensity function at the given state.
   */
  FloatingPointType evaluateIntensityAtState(
      const BpsState<FloatingPointType, Dimensionality>& state) const;

  /**
   * Returns the bounce operator used by this algorithm.
   */
  BounceOperator<FloatingPointType, Dimensionality>*
    getBounceOperator() const;

  /**
   * Returns the energy gradient function.
   */
  std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
                Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>
    getEnergyGradient() const;

 protected:

  std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      getInitialState() const override;

 private:

  Eigen::Matrix<FloatingPointType, Dimensionality, 1> getRefreshedVelocity()
    const;

  FloatingPointType refreshRate_;

  const std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
    Eigen::Matrix<FloatingPointType, Dimensionality, 1>)> energyGradient_;

  const std::unique_ptr<BounceOperator<FloatingPointType, Dimensionality>>
    bounceOperator_;

  std::shared_ptr<BpsFlow<FloatingPointType, Dimensionality>> flow_;

  const std::unique_ptr<BpsPoissonProcessStrategy<
    FloatingPointType, Dimensionality>> ppStrategy_;
};

}

#include "bouncy_particle_sampler/basic_bps.tcc"
