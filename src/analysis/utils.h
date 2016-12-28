#pragma once

#include <functional>

namespace bps {
namespace analysis {

/**
 * Helper functions for analyzing performance of the algorithms.
 */
class AnalysisUtils {

 public:

  /**
   * Measures the execution time of the given function in milliseconds.
   */
  template<typename Function, typename... Args>
  static double getExecutionTime(Function function, Args&&... args);

  /**
   * Returns the result of function which time is being measured.
   * In this method the time is set to the time variable passed by reference.
   */
  template<typename ResultType, typename...Args>
  static ResultType getExecutionTimeAndFunctionResult(
    const std::function<ResultType(Args...)>& function,
    double& time,
    Args&&... args);

};

}
}

#include "analysis/utils.tcc"
