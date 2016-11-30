#pragma once
#include <iostream>
#include "gsl_wrappers/wrappers_utils.h"

#include <gsl/gsl_integration.h>

namespace {

  // Size n allocates workspace sufficient to hold n double precision
  // intervals, their integration results and error estimates.
  const int kSizeOfIntegrationWorkspace = 10000;

  // Default absolute error tolerance is set to 0, because we only want
  // to set the relative error tolerance.
  const double kDefaultEpsabs = 0;

  // Default value for the relative error of integration.
  const double kDefaultEpsrel = 1e-3;
}

namespace bps {

// We wrap the given std::function into gsl_function and perform
// type conversions between T and double where appropriate.
// The GSL integration API can be found at https://www.gnu.org/software/gsl
template<typename T>
T GslIntegrationWrappers<T>::integrate(
    const std::function<T(T)>& function, T a, T b) {

  gsl_function gsl_func = bps::GslWrappersUtils<T>
      ::convertStdFunctionToGslFunction(function);
  gsl_integration_workspace* w = gsl_integration_workspace_alloc(
      kSizeOfIntegrationWorkspace);

  double result, error;
  gsl_integration_qag(
      &gsl_func,
      (double) a,
      (double) b,
      kDefaultEpsabs,
      kDefaultEpsrel,
      GSL_INTEG_GAUSS61,
      kSizeOfIntegrationWorkspace,
      w,
      &result,
      &error);

  gsl_integration_workspace_free(w);

  return (T) result;
}

}
