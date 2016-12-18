#pragma once

#include "gsl_wrappers/wrappers_utils.h"

#include <gsl/gsl_roots.h>
#include <gsl/gsl_errno.h>

namespace bps {

template<typename T>
T GslRootFindingWrappers<T>::brentSolver(
    const std::function<T(T)>& function, T a, T b) {

  gsl_set_error_handler_off();

  const gsl_root_fsolver_type* solverType = gsl_root_fsolver_brent;
  gsl_root_fsolver* solver = gsl_root_fsolver_alloc(solverType);

  gsl_function gsl_func = bps::GslWrappersUtils<T>
      ::convertStdFunctionToGslFunction(function);

  gsl_root_fsolver_set(solver, &gsl_func, a, b);

  do {
    gsl_root_fsolver_iterate(solver);
    a = gsl_root_fsolver_x_lower(solver);
    b = gsl_root_fsolver_x_upper(solver);
  } while (gsl_root_test_interval(a, b, 0, 1e-7) == GSL_CONTINUE);

  double root = gsl_root_fsolver_root(solver);
  gsl_root_fsolver_free(solver);

  return (T) root;
}

}

