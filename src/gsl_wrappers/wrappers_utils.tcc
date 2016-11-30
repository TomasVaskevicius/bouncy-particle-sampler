#pragma once

namespace bps {

template<typename T>
gsl_function GslWrappersUtils<T>::convertStdFunctionToGslFunction(
    const std::function<T(T)>& function) {

  gsl_function gsl_func;
  gsl_func.function = [](double x, void* params) -> double {
    return (*static_cast<std::function<T(T)>*>(params))((T) x);
  };
  gsl_func.params = const_cast<std::function<T(T)>*>(&function);

  return gsl_func;
}

}
