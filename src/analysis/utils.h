#pragma once

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

};

}
}

#include "analysis/utils.tcc"
