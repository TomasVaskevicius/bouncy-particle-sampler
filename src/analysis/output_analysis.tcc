#pragma once

#include "bouncy_particle_sampler/bps_utils.h"
#include "bouncy_particle_sampler/estimators/integrated_autocorrelation_time.h"

#include <algorithm>

namespace bps {
namespace analysis {

template<typename T, int Dim>
std::vector<std::vector<T>> OutputAnalysis<T, Dim>
    ::calculateExpectationEstimates(
      const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns,
      const typename Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
      const typename OutputAnalysis<T, Dim>::ExpecatationEstimator&
        expectationEstimator) {

  std::vector<std::vector<T>> expectationEstimates;

  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  expectationEstimates.push_back(
                    expectationEstimator(sampleRun, functionToEstimate));
                });

  return expectationEstimates;
}

template<typename T, int Dim>
std::vector<T> OutputAnalysis<T, Dim>::calculateIntegratedAutocorrelationTime(
    const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns,
    const typename Mcmc<T, Dim>::RealFunctionOnSamples& functionToEstimate,
    const typename OutputAnalysis<T, Dim>::ExpecatationEstimator&
      expectationEstimator,
    const int& numberOfBatches) {

  std::vector<T> iactEstimates;

  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  iactEstimates.push_back(
                    IntegratedAutocorrelationTime<T, Dim>
                      ::calculateIntegratedAutocorrelationTime(
                        sampleRun, functionToEstimate, numberOfBatches));
                });

  return iactEstimates;
}

template<typename T, int Dim>
std::vector<T> OutputAnalysis<T, Dim>::calculateAverageIntereventTime(
    const typename OutputAnalysis<T, Dim>::SampleOutputsVector& sampleRuns) {

  std::vector<T> averageIntereventTimes;

  std::for_each(sampleRuns.begin(),
                sampleRuns.end(),
                [&] (const typename Mcmc<T, Dim>::SampleOutput& sampleRun) {
                  T totalPathLength =
                    BpsUtils<T, Dim>::getTotalPathLength(sampleRun);
                  int numberOfEvents = sampleRun.size();
                  averageIntereventTimes.push_back(
                    totalPathLength / (numberOfEvents - 1));
                });

  return averageIntereventTimes;
}

}
}

