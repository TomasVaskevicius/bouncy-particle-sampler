#include <iostream>
#include <vector>

#include <Eigen/Core>

#include "mcmc/zig_zag/zig_zag_builder.h"
#include "mcmc/distributions/gaussian.h"

#include "run_and_plot.h"

using namespace pdmp::mcmc;

int main(int argc, char **argv) {
  // Create a standard normal 2-dimensional factor.
  RealMatrix covariances(2, 2);
  covariances << 1, 0,
                 0, 1;
  RealVector mean(2);
  mean << 0, 0;
  mcmc::GaussianDistribution gaussianDistribution(mean, covariances);

  // Create a Zig-Zag sampler from the factor created above.
  pdmp::mcmc::ZigZagBuilder zigZagBuilder(2);
  zigZagBuilder.addFactor(
    {0,1},
    gaussianDistribution);
  auto pdmp = zigZagBuilder.build();

  // Finally, run the sampler and plot its path.
  runSamplerAndPlotPath(pdmp);
  return 0;
}

