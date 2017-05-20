#include <gtest/gtest.h>

#include <Eigen/Core>
#include <stan/math/prim/mat.hpp>

#include "core/policies/linear_flow.h"
#include "mcmc/utils.h"
#include "mcmc/distributions/gaussian.h"

using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;


TEST(TestGradientOfLogDensity, CalculateGradientOfLogPdfOfNormalDistribution) {
  RealVector mean(2);
  mean << 0, 0;
  RealMatrix covariances(2, 2);
  covariances << 1, 0, 0, 1;
  pdmp::mcmc::GaussianDistribution gaussianDistribution(mean, covariances);
  auto logPdfGradient = pdmp::mcmc::getGradientOfAFunctor(
    gaussianDistribution.getLogPdf());
  RealVector x(2);
  x << 2.5, 3.5;
  EXPECT_FLOAT_EQ(logPdfGradient(x)(0), -x(0));
  EXPECT_FLOAT_EQ(logPdfGradient(x)(1), -x(1));
}

TEST(
  TestGaussianPoissonProcessStrategy,
  TestJumpTimeDoesNotHappenDuringDownhillMotion) {

  RealVector mean(1);
  mean << 2;
  RealMatrix covariances(1, 1);
  covariances << 1;
  pdmp::mcmc::GaussianDistribution gaussianDistribution(mean, covariances);
  auto poissonProcessStrategy =
    gaussianDistribution.getPoissonProcessStrategy<pdmp::LinearFlow>();

  RealVector start(2);
  start << mean(0) + 1000.0, -1.0;

  // Particle moving downwards to the left, should only once starts going up.
  auto jumpTime = poissonProcessStrategy(start, 0, 0);
  EXPECT_TRUE(start(0) + start(1) * jumpTime->time < mean(0));

  // Symmetric test from the other side.
  start << mean(0) - 1000.0, 1.0;
  jumpTime = poissonProcessStrategy(start, 0, 0);
  EXPECT_TRUE(start(0) + start(1) * jumpTime->time > mean(0));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

