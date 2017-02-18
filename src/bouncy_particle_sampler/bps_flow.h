#pragma once

#include "bouncy_particle_sampler/bps_state.h"

namespace bps {

/**
 * This anstract class separates the Piecewise Deterministic Markov Process
 * flow from the BPS implementation.
 */
template<typename FloatingPointType, int Dimensionality>
class BpsFlow {

 public:

  ~BpsFlow() {}

  virtual BpsState<FloatingPointType, Dimensionality>
    advanceStateByFlowFunction(
      const BpsState<FloatingPointType, Dimensionality>& initialState,
      const FloatingPointType& time) const = 0;

};

}
