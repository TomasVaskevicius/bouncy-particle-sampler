#include <iostream>
#include <vector>

#include <Eigen/Core>

#include "analysis/pdmp_runner.h"
#include "analysis/running_policies/fixed_iterations_count_runner.h"
#include "analysis/output_processors/batch_means.h"
#include "analysis/output_processors/path_collector.h"
#include "analysis/plotting/piecewise_linear_path.h"

#include "core/policies/linear_flow.h"
#include "core/state_space/position_and_velocity_state.h"

#include "mcmc/bps/bps_builder.h"
#include "mcmc/distributions/gaussian.h"

using namespace pdmp;
using namespace pdmp::analysis;
using namespace pdmp::mcmc;
using namespace std;

using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
using State = DynamicPositionAndVelocityState<double>;

const int kNumberOfIterations = 100000;
const double kRefreshRate = 1.0;

void generatePathAndPlot() {
  // Create a standard normal 2-dimensional factor.
  RealMatrix covariances(2, 2);
  covariances << 1, 0,
                 0, 1;
  RealVector mean(2);
  mean << 0, 0;
  mcmc::GaussianDistribution gaussianDistribution(mean, covariances);

  // Create a BPS sampling from the factor created above.
  pdmp::mcmc::BpsBuilder bpsBuilder(2);
  bpsBuilder.addFactor(
    {0,1},
    gaussianDistribution,
    kRefreshRate);
  auto pdmp = bpsBuilder.build();

  // Create a runner which will run the process for a fixed number of iterations.
  auto runner = PdmpRunner<decltype(pdmp), State, FixedIterationsCountRunner>();

  // Collect the path for plotting.
  auto pathCollector = PathCollector<decltype(pdmp), State>();
  runner.registerAnObserver(&pathCollector);

  // Set an initial state.
  State initial(
    (RealVector(2) << 1, 1).finished(),
    (RealVector(2) << -1, 2).finished());

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

int main(int argc, char **argv) {
  generatePathAndPlot();
  return 0;
}

