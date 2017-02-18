#pragma once

#include "bouncy_particle_sampler/linear_flow.h"

namespace bps {

template<typename T, int Dim>
BpsPoissonProcessStrategy<T, Dim>::BpsPoissonProcessStrategy(
  const EnergyGradient& energyGradient)
  : energyGradient_(energyGradient),
    flow_(std::shared_ptr<BpsFlow<T, Dim>>(new BpsLinearFlow<T, Dim>())) {
}

}
