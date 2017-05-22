#pragma once

#include "analysis/pdmp_runner.h"
#include "analysis/running_policies/fixed_iterations_count_runner.h"
#include "analysis/output_processors/batch_means.h"
#include "analysis/output_processors/path_collector.h"
#include "analysis/plotting/piecewise_linear_path.h"

#include "core/state_space/position_and_velocity_state.h"

using namespace pdmp;
using namespace pdmp::analysis;
using namespace std;

using State = DynamicPositionAndVelocityState<double>;
using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;

const int kNumberOfIterations = 1000;

/**
 * Runs the given sampler and plots its 2-D path.
 */
template<class Pdmp>
void runSamplerAndPlotPath(Pdmp& pdmp) {
  // Create a runner which will run the process for a fixed number of iterations.
  auto runner = PdmpRunner<
    Pdmp, State, FixedIterationsCountRunner>();

  // Collect the path for plotting.
  auto pathCollector = PathCollector<Pdmp, State>();
  runner.registerAnObserver(&pathCollector);

  // Set an initial state.
  State initial(
    (RealVector(2) << 1, 1).finished(),
    (RealVector(2) << -1, 1).finished());

  // Run the process.
  runner.run(pdmp, initial, kNumberOfIterations);

  // Take the collected samples and plot the 2-d path.
  std::vector<State> path = pathCollector.getCollectedStates();
  std::vector<double> x, y;
  for (int i = 0;i < path.size(); i++) {
    x.push_back(path[i].position(0));
    y.push_back(path[i].position(1));
  }
  plotTwoDimensionalPiecewiseLinearSamplePath(x, y, "path");
}
