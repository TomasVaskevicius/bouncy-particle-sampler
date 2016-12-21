#pragma once

#include "core/mcmc.h"

#include <Eigen/Core>
#include <functional>
#include <vector>

namespace bps {
namespace analysis {

template<typename FloatingPointType, int Dimensionality>
class OutputAnalysis {

 public:

  typedef std::vector<
            typename Mcmc<FloatingPointType, Dimensionality>::SampleOutput>
          SampleOutputsVector;


  typedef std::function<std::vector<FloatingPointType>(
            const typename Mcmc<FloatingPointType, Dimensionality>::SampleOutput&,
            const typename Mcmc<FloatingPointType, Dimensionality>
              ::RealFunctionOnSamples&)>
          ExpecatationEstimator;

  /**
   * Generates expectation estimation statistics for a given vector
   * of results from multiple runs of the BPS algorithm.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @param functionToEstimate
   *   A function which respect to which the expectation needs to be calculated.
   * @param expecatationEstimator
   *   Expectation estimator that should be used for each of the sample runs.
   * @return
   *   A vector of expectation estimates, for each of the sample runs.
   */
  static std::vector<std::vector<FloatingPointType>>
    calculateExpectationEstimates(
      const SampleOutputsVector& sampleRuns,
      const typename Mcmc<FloatingPointType, Dimensionality>
        ::RealFunctionOnSamples& functionToEstimate,
      const ExpecatationEstimator& expectationEstimator);

  /**
   * For each given vector of sample outputs, calculates the integrated
   * autocorrelation time using the given estimator. IACT is calculated
   * by using the Batch Means method.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @param functionToEstimate
   *   A function which respect to which the expectation needs to be calculated.
   * @param expectationEstimator
   *   A function which will be applied on the smaples.
   * @param numberOfBatches
   *   The number of batches to use in the Batch Means method.
   * @return
   *   A vector of integrated autocorrelation times for each given sample run.
   */
  static std::vector<FloatingPointType>
    calculateIntegratedAutocorrelationTime(
      const SampleOutputsVector& sampleRuns,
      const typename Mcmc<FloatingPointType, Dimensionality>
        ::RealFunctionOnSamples& functionToEstimate,
      const ExpecatationEstimator& expectationEstimator,
      const int& numberOfBatches = 100);

  /**
   * Calculates the average inter event time for given BPS runs.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @return
   *   A vector of average inter event times for each given sample run.
   */
  static std::vector<FloatingPointType>
    calculateAverageIntereventTime(const SampleOutputsVector& sampleRuns);

  /**
   * For each given sample run, returns a vector of event times.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @return
   *   A vector of event times (in ascending order) for each sample run.
   */
  static std::vector<std::vector<FloatingPointType>>
    getEventTimes(const SampleOutputsVector& sampleRuns);
};

}
}

#include "analysis/output_analysis.tcc"

