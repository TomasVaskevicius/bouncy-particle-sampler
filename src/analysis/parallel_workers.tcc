#pragma once

#include "analysis/utils.h"

#include <thread>

namespace bps {
namespace analysis {

template<typename ResultType>
std::unique_ptr<std::vector<ResultType>> ParallelWorkers<ResultType>
  ::executeTasksInParallel(
    const std::function<ResultType(void)>& task,
    const int& numberOfRepetitions,
    const int& numberOfThreads,
    std::vector<double>* executionTimes) {

  auto resultsVector = std::unique_ptr<std::vector<ResultType>>(
      new std::vector<ResultType>());

  std::vector<std::thread> activeThreads;
  std::atomic_int numberOfTasksStarted(0);
  for (int i = 0; i < numberOfThreads; i++) {
    activeThreads.push_back(std::thread(
        &ParallelWorkers<ResultType>::worker,
        this,
        std::ref(numberOfRepetitions),
        std::ref(*resultsVector),
        std::ref(numberOfTasksStarted),
        std::ref(task),
        executionTimes));
  }

  for (auto& thread : activeThreads) {
    thread.join();
 }

  return resultsVector;
}

template<typename ResultType>
void ParallelWorkers<ResultType>::worker(
  const int& requestedNumberOfTasks,
  std::vector<ResultType>& sharedResultsVector,
  std::atomic_int& numberOfTasksStarted,
  const std::function<ResultType(void)>& task,
  std::vector<double>* executionTimes) {

  while (numberOfTasksStarted.load() < requestedNumberOfTasks) {
    int localNumberOfTasksStarted = numberOfTasksStarted.load();
    if (numberOfTasksStarted.compare_exchange_strong(
            localNumberOfTasksStarted, localNumberOfTasksStarted + 1)) {

      double executionTime;
      ResultType myResult = AnalysisUtils::getExecutionTimeAndFunctionResult(
        task, executionTime);

      this->executeSequentially(
          [&myResult, &sharedResultsVector, &executionTime, &executionTimes]
          () -> void {
            sharedResultsVector.push_back(myResult);
            if (executionTimes != nullptr) {
              executionTimes->push_back(executionTime);
            }
          });
    }
  }
}

template<typename ResultType>
void ParallelWorkers<ResultType>
  ::executeSequentially(std::function<void()> function) {

  this->sequentialExecutionMutex_.lock();
  function();
  this->sequentialExecutionMutex_.unlock();
}


}
}
