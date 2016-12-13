#pragma once

#include "bouncy_particle_sampler/bps_state.h"
#include "bouncy_particle_sampler/bps_utils.h"
#include "gsl_wrappers/integration_wrappers.h"

namespace bps {

template<typename T, int Dim>
std::vector<T> BpsExpectationEstimators<T, Dim>::numericalIntegrationEstimator(
    const std::vector<std::shared_ptr<McmcState<T, Dim>>>& samples,
    const std::function<T(Eigen::Matrix<T, Dim, 1>)>& function) {

  std::vector<T> expectationEstimates;

  T totalTime = (T) 0.0;
  T totalIntegral = (T) 0.0;
  for (int i = 1; i < samples.size(); i++) {
    totalTime += BpsUtils<T, Dim>::getTimeDifferenceBetweenBpsMcmcStates(
        samples[i-1], samples[i]);
    totalIntegral += BpsUtils<T, Dim>::integrateAlongPiecewiseLinearSegment(
        samples[i-1], samples[i], function);

    expectationEstimates.push_back(totalIntegral / totalTime);
  }

  return expectationEstimates;
}

}
