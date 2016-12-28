#pragma once

#include "bouncy_particle_sampler/bps_state.h"
#include "bouncy_particle_sampler/bps_utils.h"
#include "gsl_wrappers/integration_wrappers.h"

namespace bps {

template<typename T, int Dim>
std::vector<T> BpsExpectationEstimators<T, Dim>::numericalIntegrationEstimator(
  const typename Mcmc<T, Dim>::SampleOutput& samples,
  const typename Mcmc<T, Dim>::RealFunctionOnSamples& function) {

  std::vector<T> expectationEstimates;
  expectationEstimates.push_back(0.0);

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


template<typename T, int Dim>
std::vector<T> BpsExpectationEstimators<T, Dim>::embeddedChainEstimator(
  const typename Mcmc<T, Dim>::SampleOutput& samples,
  const typename Mcmc<T, Dim>::RealFunctionOnSamples& function,
  const BasicBps<T, Dim>& algorithm) {

  std::vector<T> expectationEstimates;
  expectationEstimates.push_back(0.0);

  T refreshRate = algorithm.getRefreshRate();
  BounceOperator<T, Dim>* bounceOperator = algorithm.getBounceOperator();

  T functionEstimatesScaledByIntensity = (T) 0.0;
  T sumOfInvertedIntensities = (T) 0.0;
  for (int i = 1; i < samples.size(); i++) {
    T functionAtParticleLocation =
      function(samples[i]->getVectorRepresentedByState());

    // TODO: getting this should be refactored to separate PDMP class.
    T intensity = algorithm.evaluateIntensityAtState(
      *(bounceOperator->getStateAfterBounce(
        *static_cast<BpsState<T, Dim>*>(samples[i].get()),
        algorithm.getEnergyGradient())));
    intensity += algorithm.getRefreshRate();

   functionEstimatesScaledByIntensity += functionAtParticleLocation / intensity;
   sumOfInvertedIntensities += ((T) 1.0) / intensity;

   expectationEstimates.push_back(
     functionEstimatesScaledByIntensity / sumOfInvertedIntensities);
  }

  return expectationEstimates;
}

}
