#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace bps {
namespace analysis {

/**
 * A helper class for executing independent tasks in parallel.
 */
template<typename ResultType>
class ParallelWorkers {

 public:

  /**
   * Executes some task a requested number of times using a given
   * number of threads.
   *
   * @param task
   *   A function which should be called by worker to execute the requested
   *   task. The given function should not hold any state and calls to this
   *   function should be independent of each other.
   * @param numberOfRepetitios
   *   The number of times the given task needs to be done.
   * @param numberOfThreads
   *   The number of threads we should use to perform the given tasks.
   * @param executionTimes
   *   If not nullptr, fills in the given vector with the execution times
   *   for each task.
   * @return
   *   A vector of results. The size of the vector is equal to the
   *   numberOfRepetitions parameter.
   */
  std::unique_ptr<std::vector<ResultType>> executeTasksInParallel(
      const std::function<ResultType(void)>& task,
      const int& numberOfRepetitions,
      const int& numberOfThreads,
      std::vector<double>* executionTimes = nullptr);

 private:

  // A worker for performing the given task.
  void worker(
      const int& requestedNumberOfTasks,
      std::vector<ResultType>& sharedResultsVector,
      std::atomic_int& numberOfTasksStarted,
      const std::function<ResultType(void)>& task,
      std::vector<double>* executionTimes);

  // Will be used by executeSequentially method.
  std::mutex sequentialExecutionMutex_;

  // All calls to this method will be executed sequentially,
  // so that the execution of given functinos do not overlap.
  void executeSequentially(std::function<void()> function);

};

}
}

#include "parallel_workers.tcc"
