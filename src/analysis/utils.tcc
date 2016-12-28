#pragma once

#include <chrono>
#include <utility>

namespace bps {
namespace analysis {

template<typename F, typename... Args>
double AnalysisUtils::getExecutionTime(F function, Args&&... args) {
  std::chrono::high_resolution_clock::time_point t1
    = std::chrono::high_resolution_clock::now();

  function(std::forward<Args>(args)...);

  std::chrono::high_resolution_clock::time_point t2
    = std::chrono::high_resolution_clock::now();

  return std::chrono::duration_cast<
    std::chrono::milliseconds>(t2 - t1).count();
}


template<typename ResultType, typename...Args>
ResultType AnalysisUtils::getExecutionTimeAndFunctionResult(
  const std::function<ResultType(Args...)>& function,
  double& time,
  Args&&... args) {

  ResultType result;
  auto wrappedFunction = [&] (Args... args) -> void {
    result = function(std::forward<Args>(args)...);
  };

  time = AnalysisUtils::getExecutionTime(
    wrappedFunction, std::forward<Args>(args)...);

  return result;
}

}
}
