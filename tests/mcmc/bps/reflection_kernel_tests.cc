#include <gtest/gtest.h>

#include <Eigen/Core>

#include "mcmc/utils.h"
#include "mcmc/bps/reflection_kernel.h"
#include "mcmc/distributions/gaussian.h"

using namespace pdmp::mcmc;

using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

class ReflectionKernelTests : public ::testing::Test {

 protected:

  ReflectionKernelTests()
    : mean_((RealVector(2) << 0, 0).finished()),
      covariances_((RealMatrix(2, 2) << 1, 0, 0, 1).finished()),
      gaussianDistribution_(mean_, covariances_),
      initialState_((RealVector(4) << 10.0, 5.6, 34.4, -1032.33).finished()) {

    initialPosition_ = initialState_.head(2);
    initialVelocity_ = initialState_.tail(2);
  }

  RealVector mean_;
  RealMatrix covariances_;
  GaussianDistribution gaussianDistribution_;
  RealVector initialState_;
  RealVector initialPosition_;
  RealVector initialVelocity_;
};

TEST_F(ReflectionKernelTests, TestReflectionDoesNotChangeNorm) {
  auto reflectionKernel = getReflectionKernel(
    getGradientOfAFunctor(gaussianDistribution_.getLogPdf()));
  auto reflected = reflectionKernel(initialState_);
  auto reflectedVelocity = reflected.tail(2);
  EXPECT_DOUBLE_EQ(initialVelocity_.norm(), reflectedVelocity.norm());
}

TEST_F(ReflectionKernelTests, TestReflectionIsSelfInverseOperation) {
  auto reflectionKernel = getReflectionKernel(
    getGradientOfAFunctor(gaussianDistribution_.getLogPdf()));
  auto reflectedTwice = reflectionKernel(reflectionKernel(initialState_));
  auto reflectedTwiceVelocity = reflectedTwice.tail(2);
  EXPECT_DOUBLE_EQ(initialVelocity_(0), reflectedTwiceVelocity(0));
  EXPECT_DOUBLE_EQ(initialVelocity_(1), reflectedTwiceVelocity(1));
}

TEST_F(ReflectionKernelTests, TestReflectionDoesNotChangePosition) {
  auto reflectionKernel = getReflectionKernel(
    getGradientOfAFunctor(gaussianDistribution_.getLogPdf()));
  auto reflected = reflectionKernel(initialState_);
  auto reflectedPosition = reflected.head(2);
  EXPECT_DOUBLE_EQ(initialPosition_(0), reflectedPosition(0));
  EXPECT_DOUBLE_EQ(initialPosition_(1), reflectedPosition(1));
}

TEST_F(ReflectionKernelTests, TestSimpleReflection) {
  auto reflectionKernel = getReflectionKernel(
    getGradientOfAFunctor(gaussianDistribution_.getLogPdf()));
  RealVector state = (RealVector(4) << 1, 1, 2, 0).finished();
  // Energy gradient at position (1, 1). Particle is going to the right,
  // Should be reflected at the same speed with 90 degree angle to the bottom.
  auto reflected = reflectionKernel(state);
  auto reflectedVelocity = reflected.tail(2);
  EXPECT_DOUBLE_EQ(reflectedVelocity(0), 0.0);
  EXPECT_DOUBLE_EQ(reflectedVelocity(1), -2.0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

