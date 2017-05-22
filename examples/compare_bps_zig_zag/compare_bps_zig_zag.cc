#include <iostream>

#include <Eigen/Core>

#include "analysis/plotting/box_plot.h"
#include "analysis/pdmp_runner.h"
#include "analysis/output_processors/batch_means.h"
#include "analysis/running_policies/timed_runned.h"
#include "analysis/timers/per_thread_cpu_timer.h"
#include "analysis/parallel_workers.h"

#include "core/state_space/position_and_velocity_state.h"
#include "core/policies/linear_flow.h"

#include "mcmc/bps/bps_builder.h"
#include "mcmc/distributions/gaussian.h"
#include "mcmc/zig_zag/zig_zag_builder.h"
#include "mcmc/utils.h"

#include <stan/math/prim/mat.hpp>

using namespace pdmp;
using namespace pdmp::analysis;
using namespace pdmp::mcmc;
using namespace std;

using State = DynamicPositionAndVelocityState<double>;
using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

const int kDimension = 2;
const RealVector kMean = RealVector::Zero(kDimension);
const RealMatrix kCovariances = RealMatrix::Identity(kDimension, kDimension);
GaussianDistribution gaussian(kMean, kCovariances);

auto getGaussian() {
  GaussianDistribution copy = gaussian;
  return copy;
}

auto functionToEstimate = [] (const auto& state) {
  return state.position.squaredNorm();
};

// Chooses a position based on standard normal rng.
// Initialises velocity to 1, 1, ..., 1.
auto getInitialState() {
  auto rng = getRng();
  State initialState(
    stan::math::multi_normal_rng(kMean, kCovariances, rng),
    RealVector(kDimension));
  for (int i = 0; i < kDimension; i++) {
    initialState.velocity(i) = 1.0;
  }
  return initialState;
}

// Returns a new BPS pdmp.
auto buildBpsPdmp(GaussianDistribution& distribution) {
  BpsBuilder bpsBuilder(kDimension);
  std::vector<int> modelVariables;
  for (int i = 0; i < kDimension; i++) {
    modelVariables.push_back(i);
  }
  bpsBuilder.addFactor(modelVariables, distribution, 1.0);
  return bpsBuilder.build();
}

// Returns a new ZigZag pdmp.
auto buildZigZagPdmp(GaussianDistribution& distribution) {
  ZigZagBuilder zigZagBuilder(kDimension);
  std::vector<int> modelVariables;
  for (int i = 0; i < kDimension; i++) {
    modelVariables.push_back(i);
  }
  zigZagBuilder.addFactor(modelVariables, distribution);
  return zigZagBuilder.build();
}

// Estimates the asymptotic variance for a given pdmp.
template<class Pdmp>
double getAsymptoticVariance(Pdmp& pdmp) {
  PdmpRunner<Pdmp, State, TimedRunner<PerThreadCpuTimer>> runner;
  auto batchMeansProcessor = BatchMeans<Pdmp, State, LinearFlow>(
    functionToEstimate);
  runner.registerAnObserver(&batchMeansProcessor);
  auto initialState = getInitialState();
  runner.run(pdmp, initialState, 10000, 0, false);
  return batchMeansProcessor.estimateAsymptoticVariance();
}

double getBpsAsymptoticVariance() {
  auto g = getGaussian();
  auto bpsPdmp = buildBpsPdmp(g);
  return getAsymptoticVariance(bpsPdmp);
}

double getZigZagAsymptoticVariance() {
  auto zigZagPdmp = buildZigZagPdmp(gaussian);
  return getAsymptoticVariance(zigZagPdmp);
}

int main() {
  ::bps::analysis::ParallelWorkers<double> bpsWorkers;
  ::bps::analysis::ParallelWorkers<double> zigZagWorkers;

  auto bpsAsymptoticVariances = bpsWorkers.executeTasksInParallel(
    getBpsAsymptoticVariance, 42, 7);
  auto zigZagAsymptoticVariances = zigZagWorkers.executeTasksInParallel(
    getZigZagAsymptoticVariance, 42, 7);

  plotBoxPlot({*bpsAsymptoticVariances, *zigZagAsymptoticVariances},
              {"Bps", "Zig-Zag"},
              "asymptoticvar");
  return 0;
}

