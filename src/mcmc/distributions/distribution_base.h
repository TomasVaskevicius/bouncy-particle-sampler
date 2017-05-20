#pragma once

#include <random>

#include <Eigen/Core>

#include "core/policies/poisson_process.h"

namespace pdmp {
namespace mcmc {

/**
 * A base class for representing all distributions.
 * Each distribution holds its log probability density function and
 * a poisson process strategy function.
 */
template<class Derived>
class DistributionBase {

 public:

  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
  using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

  /**
   * Returns the log probability density of the represented distribution.
   * The returned functor computes the log pdf up to an additive constant.
   */
  auto getLogPdf() const;

  template<class Flow, class... Args>
  auto getPoissonProcessStrategy(Args&&... args) const;

};

}
}

#include "distribution_base.tcc"
