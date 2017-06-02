#include <iostream>

#include <Eigen/Core>

#include "analysis/pdmp_runner.h"
#include "analysis/running_policies/timed_runned.h"
#include "analysis/timers/wall_clock_timer.h"
#include "analysis/parallel_workers.h"

#include "core/state_space/position_and_velocity_state.h"
#include "core/policies/linear_flow.h"

#include "mcmc/bps/bps_builder.h"
#include "mcmc/distributions/gaussian.h"
#include "mcmc/utils.h"

#include <stan/math/prim/mat.hpp>

#include <gflags/gflags.h>

#include "variance_processor.h"

using namespace pdmp;
using namespace pdmp::analysis;
using namespace pdmp::mcmc;
using namespace std;

using State = DynamicPositionAndVelocityState<double>;
using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

DEFINE_int32(pairs, 100, "The chain length.");
DEFINE_int64(timeInMs, 10000, "The running time in milliseconds");
DEFINE_int32(variancesOutputCount, 0,
             "Output estimated variances for the first n variables.");


// Will be set by the initialise() function.
vector<double> realVariances;
RealMatrix chainFactorCovarianceMatrix(2, 2);
RealMatrix fullCovariancesMatrix;

// Computes the full covariance matrix, real variances and the covariances
// matrix for 2-d components of the chain.
void initialise() {
  RealMatrix precisionMatrix(2, 2);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      if (i == j) {
        precisionMatrix(i, j) = 1.0;
      } else {
        precisionMatrix(i, j) = 0.5;
      }
    }
  }

  RealMatrix fullPrecisionMatrix
    = RealMatrix::Zero(FLAGS_pairs + 1, FLAGS_pairs + 1);
  for (int i = 0; i < FLAGS_pairs; i++) {
    fullPrecisionMatrix.block(i, i, 2, 2) += precisionMatrix.block(0, 0, 2, 2);
  }

  chainFactorCovarianceMatrix = precisionMatrix.inverse();
  fullCovariancesMatrix = fullPrecisionMatrix.inverse();

  for (int i = 0; i < FLAGS_pairs + 1; i++) {
    realVariances.push_back(fullCovariancesMatrix(i, i));
  }
}

// Chooses a position and velocity from standard normal rng.
auto getInitialState() {
  auto rng = getRng();
  RealVector mean = RealVector::Zero(FLAGS_pairs + 1);
  State initialState(
    stan::math::multi_normal_rng(mean, fullCovariancesMatrix, rng),
    stan::math::multi_normal_rng(mean, fullCovariancesMatrix, rng));
  return initialState;
}

// Returns a new BPS pdmp on the gaussian chain target distribution.
auto buildBpsOnGaussianChain() {
  double perFactorRefreshRate = 1.0 / FLAGS_pairs;

  RealVector mean = RealVector::Zero(2);
  GaussianDistribution gaussian(mean, chainFactorCovarianceMatrix);

  BpsBuilder bpsBuilder(FLAGS_pairs + 1);
  for (int i = 0; i < FLAGS_pairs; i++) {
    bpsBuilder.addFactor({i, i + 1}, gaussian, perFactorRefreshRate);
  }

  return bpsBuilder.build();
}

void runBps() {
  auto pdmp = buildBpsOnGaussianChain();
  VarianceProcessor<decltype(pdmp), State> varianceProcessor;
  PdmpRunner<decltype(pdmp), State, TimedRunner<WallClockTimer>> runner;
  runner.registerAnObserver(&varianceProcessor);
  // Run with no burn in and include variance calculation time
  // into the total time.
  runner.run(pdmp, getInitialState(), FLAGS_timeInMs, 0, false);

  for (int i = 0; i < FLAGS_variancesOutputCount; i++) {
    cout << "i=" << i << endl
         << "real=" << realVariances.at(i) << endl
         << "estimated=" << varianceProcessor.getVarianceEstimate(i) << endl
         << "abs=" << realVariances.at(i) - varianceProcessor.getVarianceEstimate(i)
         << endl << endl;
  }
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  initialise();
  runBps();
  return 0;
}

