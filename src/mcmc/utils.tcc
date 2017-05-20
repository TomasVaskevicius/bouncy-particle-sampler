#pragma once

#include <Eigen/Core>
#include <stan/math/rev/mat.hpp>

namespace pdmp {
namespace mcmc {

std::mt19937_64 getRng() {
  std::random_device rd;
  std::mt19937_64 rng(rd());
  return rng;
}

template<class F>
auto getGradientOfAFunctor(const F& functor) {
  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;

  auto gradient = [functor] (const auto& x) {
    double fx;
    RealVector grad_fx;
    stan::math::gradient(functor, x, fx, grad_fx);
    return grad_fx;
  };

  return gradient;
}

}
}
