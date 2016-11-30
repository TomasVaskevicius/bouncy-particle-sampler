#pragma once

#include <functional>
#include <gsl/gsl_math.h>

namespace bps {

/**
 * Helper functions for gsl library C++ wrappers.
 */
template<typename FloatingPointType>
class GslWrappersUtils {

 public:

  /**
   * Wraps the std::function into gsl_function.
   */
  static gsl_function convertStdFunctionToGslFunction(
      const std::function<FloatingPointType(FloatingPointType)>& function);

};

}

#include "gsl_wrappers/wrappers_utils.tcc"
