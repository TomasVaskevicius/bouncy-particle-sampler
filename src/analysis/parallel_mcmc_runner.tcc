#pragma once

#include "analysis/parallel_workers.h"

namespace bps {
namespace analysis {

template<typename T, int Dim>
std::unique_ptr<std::vector<typename Mcmc<T, Dim>::SampleOutput>>
  ParallelMcmcRunner<T, Dim>::generateBpsSamples(
    const typename ParallelMcmcRunner<T, Dim>::BpsFactory& bpsFactory,
    T requiredTrajectoryLengths,
    int numberOfRuns,
    int numberOfThreads,
    std::vector<double>* executionTimesForEachRun) {

  ParallelWorkers<std::vector<std::shared_ptr<McmcState<T, Dim>>>>
      parallelWorkers;

  auto task =
      [&bpsFactory, &requiredTrajectoryLengths]
      () -> std::vector<std::shared_ptr<McmcState<T, Dim>>> {

    return bpsFactory()->getBatchOfMcmcStatesByTrajectoryLength(
        requiredTrajectoryLengths);
  };

  auto results = parallelWorkers.executeTasksInParallel(
      task,
      numberOfRuns,
      numberOfThreads,
      executionTimesForEachRun);

  return results;
}

}
}
