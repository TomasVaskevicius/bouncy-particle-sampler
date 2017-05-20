#pragma once

#include "mcmc/distributions/distribution_base.h"

namespace pdmp {
namespace mcmc {

class GaussianDistribution : public DistributionBase<GaussianDistribution> {

 public:

  /**
   * Creates a Gaussian distribution with the given mean and covariance
   * matrix.
   */
  GaussianDistribution(
    const RealVector& mean, const RealMatrix& covarianceMatrix);

  auto getLogPdf() const;

  template<class Flow>
  auto getPoissonProcessStrategy() const;

 private:

  RealVector mean_;
  RealMatrix precisionMatrix_;

};

}
}

#include "gaussian.tcc"
