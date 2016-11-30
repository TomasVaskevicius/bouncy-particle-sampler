#pragma once

#include <functional>

namespace bps {

/**
 * We use the GNU Scientific Library for numerical integration.
 * The GSL is written in C, hence we implement C++ wrappers to make the
 * GSL interface easier to use.
 *
 * Also please note that numerical integration in the GNU Scientific Library
 * is implemented using double precison floating points only, and hence
 * parameterizing this class with long double will introduce additional
 * errors.
 */
template<typename FloatingPointType>
class GslIntegrationWrappers {

 public:

  /**
   * Integrates the given function over the interval [a, b].
   */
  static FloatingPointType integrate(
      const std::function<FloatingPointType(FloatingPointType)>& function,
      FloatingPointType a,
      FloatingPointType b);

};

}

#include "gsl_wrappers/integration_wrappers.tcc"
