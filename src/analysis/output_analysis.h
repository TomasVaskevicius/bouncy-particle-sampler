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
   * Calculates the autocorrelation statistics for each of the sample runs.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @param functionToEstimate
   *   A function which respect to which the expectation needs to be calculated.
   * @param numberOfDatapoints
   *   Number of equally spaced lags.
   * @param expectationEstimator
   *   An estimator which should be used for calculating expectations.
   * @param lagUpperbound
   *   The lags will be calculated in the interval [0, lagUpperbound].
   * @param lagStepsize
   *   Specifies the stepsize for lags.
   * @return
   *   For each sample runs, return estimates of the autocorrelation
   *   function at equally spaced lags (number of datapoints is specified by
   *   parameter numberOfDatapoints).
   */
  static std::vector<std::vector<FloatingPointType>>
    calculateAutocorrelationFunction(
       const SampleOutputsVector& sampleRuns,
       const typename Mcmc<FloatingPointType, Dimensionality>
         ::RealFunctionOnSamples& functionToEstimate,
       const ExpecatationEstimator& expectationEstimator,
       const int& lagUpperbound,
       const int& lagStepSize);

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

  /**
   * Estimates the mean from a vector of sample runs.
   * The mean is estimated separately over separate runs and then the average
   * is taken.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @param functinoToEstimate
   *   A function which will be aplied to BPS samples.
   * @param expectationEstimator
   *   An estimator used for expectation calculations.
   * @return
   *   An estimate of the mean.
   */
  static FloatingPointType estimateMeanFromSampleRuns(
       const SampleOutputsVector& sampleRuns,
       const typename Mcmc<FloatingPointType, Dimensionality>
         ::RealFunctionOnSamples& functionToEstimate,
       const ExpecatationEstimator& expectationEstimator);

  /**
   * Estimates the variance from a vector of sample runs.
   * The variance is estimated separately over separate runs and then the
   * average is taken.
   *
   * @param sampleRuns
   *   A vector of sample runs of the BPS algorithm.
   * @param functinoToEstimate
   *   A function which will be aplied to BPS samples.
   * @param expectationEstimator
   *   An estimator used for expectation calculations.
   * @param meanEstimate
   *   A mean estimate (can be obtained from estimateMeanFromSampleRuns()).
   * @return
   *   An estimate of the variance.
   */
  static FloatingPointType estimateVarianceFromSampleRuns(
       const SampleOutputsVector& sampleRuns,
       const typename Mcmc<FloatingPointType, Dimensionality>
         ::RealFunctionOnSamples& functionToEstimate,
       const ExpecatationEstimator& expectationEstimator,
       const FloatingPointType& meanEstimate);

};

}
}

#include "analysis/output_analysis.tcc"
