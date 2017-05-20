#include <gtest/gtest.h>

#include <Eigen/Core>
#include <stan/math/prim/mat.hpp>

#include "mcmc/utils.h"

using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

TEST(TestStanGradientWrapper, CalculateGradientOfLogPdfOfNormalDistribution) {
  RealVector mean(2);
  mean << 0, 0;
  RealMatrix covariances(2, 2);
  covariances << 2, 0, 0, 2;
  RealMatrix precision = covariances.inverse();
  RealVector x(2);
  x << 2.5, 3.5;
  auto logPdf = [&mean, &precision] (const auto& x) {
    return stan::math::multi_normal_prec_lpdf<true>(x, mean, precision);
  };
  auto logPdfGradient = pdmp::mcmc::getGradientOfAFunctor(logPdf);
  EXPECT_DOUBLE_EQ(logPdfGradient(x)(0), -x(0) / 2.0);
  EXPECT_DOUBLE_EQ(logPdfGradient(x)(1), -x(1) / 2.0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

