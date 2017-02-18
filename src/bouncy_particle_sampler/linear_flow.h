#pragma once

#include "bouncy_particle_sampler/bps_flow.h"
#include "bouncy_particle_sampler/bps_state.h"

namespace bps {

template<typename FloatingPointType, int Dimensionality>
class BpsLinearFlow : public BpsFlow<FloatingPointType, Dimensionality> {

 public:

  BpsState<FloatingPointType, Dimensionality> advanceStateByFlowFunction(
      const BpsState<FloatingPointType, Dimensionality>& initialState,
      const FloatingPointType& time) const override;

};

}

#include "linear_flow.tcc"
