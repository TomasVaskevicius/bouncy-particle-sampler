#include "analysis/bps_benchmark.h"
#include "analysis/parallel_mcmc_runner.h"
#include "bouncy_particle_sampler/basic_bps.h"
#include "distributions/gaussian.h"

#include <string>
#include <vector>

using namespace std;

typedef bps::BasicBps<double, 2> Bps;
typedef typename bps::analysis::ParallelMcmcRunner<double, 2>::BpsFactory
        BpsFactory;
typedef typename GaussianDistributions<double, 2>::EnergyGradient
        EnergyGradient;
typedef bps::analysis::BpsBenchmark<double, 2> BpsBenchmark;
typedef typename BpsBenchmark::RealFunctionOnSamples
        FunctionOnSamples;
typedef typename BpsBenchmark::ExpectationEstimator
        ExpectationEstimator;

EnergyGradient getEnergyGradientForTesting();

vector<BpsFactory> getBpsFactories(
  vector<string>& names, vector<string>& shortNames);

vector<FunctionOnSamples> getFunctionsOnSamples(vector<string>& names);

vector<vector<ExpectationEstimator>> getExpectationEstimators(
  vector<string>& names, const vector<BpsFactory>& bpsFactories);

int main() {
  const int numberOfRunsForEachAlgorithm = 20;
  const double requiredTrajectoryLengths = 1000.0;

  vector<string> names, shortNames;
  vector<BpsFactory> bpsFactories = getBpsFactories(names, shortNames);
  string benchmarkName = "refresh_rates_comparison_with_gaussian_target";

  BpsBenchmark benchmark(bpsFactories, names, shortNames, benchmarkName);
  benchmark.generateSamples(
    requiredTrajectoryLengths, numberOfRunsForEachAlgorithm);

  vector<string> targetFunctionsNames;
  vector<FunctionOnSamples> functionsOnSamples =
    getFunctionsOnSamples(targetFunctionsNames);

  vector<string> expectationEstimatorsNames;
  vector<vector<ExpectationEstimator>> expectationEstimators =
    getExpectationEstimators(expectationEstimatorsNames, bpsFactories);

  for (int i = 0; i < functionsOnSamples.size(); i++) {
    for (int j = 0; j < expectationEstimators.size(); j++) {
      benchmark.runBenchmark(
        expectationEstimators[j],
        functionsOnSamples[i],
        expectationEstimatorsNames[j] + "/" + targetFunctionsNames[i]);
    }
  }

  return 0;
}

EnergyGradient getEnergyGradientForTesting() {
  Eigen::Matrix<double, 2, 1> mean;
  Eigen::Matrix<double, 2, 2> covarianceMatrix;

  mean << 0, 0;
  covarianceMatrix << 1, 0,
                      0, 1;

  return GaussianDistributions<double, 2>
    ::getGaussianDistributionEnergyGradient(mean, covarianceMatrix);
}


vector<BpsFactory> getBpsFactories(
  vector<string>& names, vector<string>& shortNames) {

  EnergyGradient energyGradient = getEnergyGradientForTesting();

  std::vector<BpsFactory> bpsFactories;
  for (double refreshRate = 1e-4; refreshRate <= 10.0; refreshRate *= 10.0) {
    bpsFactories.push_back(
      [refreshRate, energyGradient] () -> std::unique_ptr<Bps> {
        return std::unique_ptr<Bps>(new Bps(refreshRate, energyGradient));
      });
  }

  names = std::vector<string>({
    "1e-4", "1e-3", "1e-2", "1e-1", "1.0", "10.0"});
  shortNames = names;

  return bpsFactories;
}

vector<FunctionOnSamples> getFunctionsOnSamples(vector<string>& names) {
  names = std::vector<string>({"x^2+y^2", "x", "y"});

  std::vector<FunctionOnSamples> functions;

  functions.push_back([] (Eigen::Matrix<double, 2, 1> sample) -> double {
    return sample(0)*sample(0) + sample(1)*sample(1);
  });
  functions.push_back([] (Eigen::Matrix<double, 2, 1> sample) -> double {
    return sample(0);
  });
  functions.push_back([] (Eigen::Matrix<double, 2, 1> sample) -> double {
    return sample(1);
  });

  return functions;
}

vector<vector<ExpectationEstimator>> getExpectationEstimators(
  vector<string>& names, const vector<BpsFactory>& bpsFactories) {

  names = std::vector<string>({
    "numerical_integration_estimator", "embedded_chain_estimator"});

  int numberOfAlgorithms = bpsFactories.size();
  vector<vector<ExpectationEstimator>> expectationEstimators;

  expectationEstimators.push_back(std::vector<ExpectationEstimator>(
    numberOfAlgorithms,
    &bps::BpsExpectationEstimators<double, 2>::numericalIntegrationEstimator));

  vector<ExpectationEstimator> embeddedChainEstimators;
  for (const auto& factory : bpsFactories) {
    // While creating embedded chain estimators, we introduce a memory leak.
    // This issue will be addressed later.
    Bps* bps = factory().release();
    embeddedChainEstimators.push_back(
    [bps] (
      const typename bps::Mcmc<double, 2>::SampleOutput& samples,
      const typename bps::Mcmc<double, 2>::RealFunctionOnSamples& function)
      -> std::vector<double> {
        return bps::BpsExpectationEstimators<double, 2>::embeddedChainEstimator(
          samples, function, *bps);
      }
    );
  }

  expectationEstimators.push_back(embeddedChainEstimators);

  return expectationEstimators;
}

