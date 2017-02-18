#pragma once

#include "bouncy_particle_sampler/pp_strategy.h"

namespace bps {

/**
 * This will be the default strategy if no strategy is provided. Uses numerical
 * integration (via Gaussian quadrature) and root finding, hence this approach
 * is very slow and should be avoided.
 */
template<typename FloatingPointType, int Dimensionality>
class NumericalPoissonProcessStrategy
  : public BpsPoissonProcessStrategy<FloatingPointType, Dimensionality> {

  typedef std::function<Eigen::Matrix<FloatingPointType, Dimensionality, 1>(
                        Eigen::Matrix<FloatingPointType, Dimensionality, 1>)>
          EnergyGradient;
 public:

  NumericalPoissonProcessStrategy(const EnergyGradient& energyGradient);

  FloatingPointType getNextEventTime(
    const BpsState<FloatingPointType, Dimensionality>& bpsState) const override;

};

}

#include "numerical_pp_strategy.tcc"
