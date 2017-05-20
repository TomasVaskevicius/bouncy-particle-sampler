#pragma once

#include "core/policies/linear_flow.h"
#include "core/state_space/position_and_velocity_state.h"
#include "mcmc/pdmp_builder_base.h"
#include "mcmc/distributions/distribution_base.h"

namespace pdmp {
namespace mcmc {

namespace bps {

using State = DynamicPositionAndVelocityState<double>;
using Flow = LinearFlow;

}

/**
 * A class for building PDMPs that simulate based on the
 * Bouncy Particle Sampler algorithm.
 */
class BpsBuilder : protected PdmpBuilderBase<bps::State, bps::Flow> {

 public:

  /**
   * Takes the number of probability model variables as input.
   */
  BpsBuilder(int numberOfModelVariables);

  /**
   * Adds a factor, with the given distribution, acting on the specified
   * model variables.
   *
   * @param Ids
   *   The variable ids (indexed from 0), on which this factor depends.
   * @param distribution
   */
  template<class Distribution>
  void addFactor(
    const std::vector<int>& variableIds,
    const DistributionBase<Distribution>& distribution,
    double refreshRate = 1.0);

  /**
   * Returns the PDMP that can be used to simulated from the constructed
   * probability model.
   */
  auto build();

 private:

  int numberOfModelVariables_;

};

}
}

#include "bps_builder.tcc"
