#pragma once

#include "core/policies/linear_flow.h"
#include "core/state_space/position_and_velocity_state.h"
#include "mcmc/pdmp_builder_base.h"
#include "mcmc/distributions/distribution_base.h"

namespace pdmp {
namespace mcmc {

namespace zig_zag {

using State = DynamicPositionAndVelocityState<double>;
using Flow = LinearFlow;

}

/**
 * A class for building PDMPs that simulate based on the
 * Bouncy Particle Sampler algorithm.
 */
class ZigZagBuilder : protected PdmpBuilderBase<zig_zag::State, zig_zag::Flow> {

 public:

  /**
   * Takes the number of probability model variables as input.
   */
  ZigZagBuilder(int numberOfModelVariables);

  /**
   * Adds a factor, with the given distribution, acting on the specified
   * model variables.
   *
   * @param Ids
   *   The variable ids (indexed from 0), on which this factor depends.
   * @param distribution
   *   The distribution for which the factor we are adding.
   */
  template<class Distribution>
  void addFactor(
    const std::vector<int>& variableIds,
    const DistributionBase<Distribution>& distribution);

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

#include "zig_zag_builder.tcc"
