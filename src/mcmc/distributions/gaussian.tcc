#pragma once

#include <cmath>
#include <stdexcept>

#include <Eigen/Core>
#include <stan/math/prim/mat.hpp>

#include "core/policies/linear_flow.h"
#include "mcmc/utils.h"

namespace {

double transformedInnerProduct(
  const Eigen::Matrix<double, Eigen::Dynamic, 1>& x,
  const Eigen::Matrix<double, Eigen::Dynamic, 1>& y,
  const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& precisionMatrix) {

  return x.transpose() * precisionMatrix * y;
}

}

namespace pdmp {
namespace mcmc {

GaussianDistribution::GaussianDistribution(
  const RealVector& mean, const RealMatrix& covarianceMatrix)
  : mean_(mean),
    precisionMatrix_(covarianceMatrix.inverse()) {
}

auto GaussianDistribution::getLogPdf() const {
  auto logPdf =
    [mean = mean_, precisionMatrix = precisionMatrix_] (const auto& x) {
      return stan::math::multi_normal_prec_lpdf<true>(x, mean, precisionMatrix);
    };
  return logPdf;
}

template<>
auto GaussianDistribution::getPoissonProcessStrategy<LinearFlow>() const {
  auto rng = getRng();
  std::uniform_real_distribution<double> unif(0.0,1.0);
  auto strategy =
    [rng, unif, mean = mean_, precisionMatrix = precisionMatrix_]
    (const auto& state, const auto&, const auto&) mutable {
      if (state.size() % 2 != 0) {
        throw std::runtime_error(
          "Gaussian distribution poisson process strategy factory was invoked "
          "using the linear flow policy, but the provided vector is of odd size"
          " " + std::to_string(state.size()) + ".");
      }
      RealVector position = state.head(state.size() / 2) - mean;
      RealVector velocity = state.tail(state.size() / 2);
      double squaredVelocityNorm = transformedInnerProduct(
        velocity, velocity, precisionMatrix);
      double xv = transformedInnerProduct(position, velocity, precisionMatrix);
      double logU = log(unif(rng));
      if (xv >= 0) {
        return dependencies_graph::wrapPoissonProcessResult(
          (-xv + sqrt(xv * xv - 2.0 * squaredVelocityNorm * logU))
          / squaredVelocityNorm);
      } else {
        return dependencies_graph::wrapPoissonProcessResult(
          (-xv + sqrt(-2.0 * squaredVelocityNorm * logU))
          / squaredVelocityNorm);
      }
    };
  return strategy;
}

}
}

