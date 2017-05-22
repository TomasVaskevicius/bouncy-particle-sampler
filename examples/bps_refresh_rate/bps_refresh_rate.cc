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

auto functionToEstimate = [] (const auto& state) {
  return state.position.squaredNorm();
};

// Chooses a position and velocity from standard normal rng.
auto getInitialState() {
  auto rng = getRng();
  State initialState(
    stan::math::multi_normal_rng(kMean, kCovariances, rng),
    stan::math::multi_normal_rng(kMean, kCovariances, rng));
  return initialState;
}

// Returns a new BPS pdmp.
auto buildBpsPdmp(GaussianDistribution& distribution, double refreshRate) {
  BpsBuilder bpsBuilder(kDimension);
  std::vector<int> modelVariables;
  for (int i = 0; i < kDimension; i++) {
    modelVariables.push_back(i);
  }
  bpsBuilder.addFactor(modelVariables, distribution, refreshRate);
  return bpsBuilder.build();
}

// Estimates the asymptotic variance for a given pdmp.
template<class Pdmp>
double getAsymptoticVariance(Pdmp& pdmp) {
  PdmpRunner<Pdmp, State, TimedRunner<PerThreadCpuTimer>> runner;
  auto batchMeansProcessor = BatchMeans<Pdmp, State, LinearFlow>(
    functionToEstimate);
  runner.registerAnObserver(&batchMeansProcessor);
  auto initialState = getInitialState();
  runner.run(pdmp, initialState, 2000, 0, true);
  return batchMeansProcessor.estimateAsymptoticVariance();
}

double getBpsAsymptoticVariance(double refreshRate) {
  auto bpsPdmp = buildBpsPdmp(gaussian, refreshRate);
  return getAsymptoticVariance(bpsPdmp);
}

int main() {
  std::vector<std::vector<double>> results;
  std::vector<std::string> names{
    "1e-2", "0.1", "1", "10", "100"};

  for (double i = 1e-2; i <= 100.0; i *= 10.0) {
    ::bps::analysis::ParallelWorkers<double> workers;
    auto task = [i] () { return getBpsAsymptoticVariance(i); };
    std::unique_ptr<std::vector<double>> workersResults =
      workers.executeTasksInParallel(task, 14, 7);
    results.push_back(*(workersResults.release()));
  }

  plotBoxPlot(results,
              names,
              "bps_refresh_rates");
  return 0;
}

