#pragma once

#include "core/mcmc.h"
#include "bouncy_particle_sampler/bounce_operator.h"
#include "bouncy_particle_sampler/bps_state.h"
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

  /**
    * Right now the most basic implementation takes only refresh rate and
    * the energyGradient as a parameter.
    */
  BasicBps(
      FloatingPointType refreshRate,
      std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
          Eigen::Matrix<FloatingPointType, Dimensionality, 1>)> energyGradient);

  std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      generateNextState() const override;

 protected:

  std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      getInitialState() const override;

 private:

  Eigen::Matrix<FloatingPointType, Dimensionality, 1> getRefreshedVelocity()
      const;

  std::unique_ptr<PoissonProcess<FloatingPointType>>
      generatePoissonProcessForState(
          const BpsState<FloatingPointType, Dimensionality>& state) const;

  Eigen::Matrix<FloatingPointType, Dimensionality, 1> calculateNewPosition(
      const BpsState<FloatingPointType, Dimensionality>& state,
      FloatingPointType time) const;

  const FloatingPointType refreshRate_;

  const std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
      Eigen::Matrix<FloatingPointType, Dimensionality, 1>)> energyGradient_;

  const std::unique_ptr<BounceOperator<FloatingPointType, Dimensionality>>
      bounceOperator_;
};

}

#include "bouncy_particle_sampler/basic_bps.tcc"