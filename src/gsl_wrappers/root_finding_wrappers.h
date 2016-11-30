#pragma once

#include <functional>

namespace bps {

template<typename FloatingPointType>
class GslRootFindingWrappers {

 public:

  /**
   * Wrapper for GSL implementation of Brent's algorithm.
   *
   * @param function
   *   The function which root we want to find.
   * @param a
   *   The lower bound of the search interval.
   * @param b
   *   The upper bound of the search interval.
   * @return
   *   An approximation for the root of the given function.
   *
   */
  static FloatingPointType brentSolver(
      const std::function<FloatingPointType(FloatingPointType)>& function,
      FloatingPointType a,
      FloatingPointType b);

};

}

#include "gsl_wrappers/root_finding_wrappers.tcc"
