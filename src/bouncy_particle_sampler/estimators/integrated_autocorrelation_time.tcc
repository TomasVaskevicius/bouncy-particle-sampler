#pragma once

#include "bouncy_particle_sampler/bps_utils.h"
#include "bouncy_particle_sampler/estimators/expectation_estimators.h"
#include "gsl_wrappers/integration_wrappers.h"

#include <cmath>
#include <limits>

// TODO:
//
// 1) Instead of assuming piecewise linear paths, refactor flow.

namespace {

// Advances the BPS state by required time using linear flow.
template<typename T, int Dim>
std::shared_ptr<bps::McmcState<T, Dim>> advancePiecewiseLinearSegment(
    const std::shared_ptr<bps::McmcState<T, Dim>>& state,
    const T& requiredLength) {

  auto location = bps::BpsUtils<T, Dim>::getLocationFromMcmcState(state);
  auto velocity = bps::BpsUtils<T, Dim>::getVelocityFromMcmcState(state);
  auto newLocation = location + velocity * requiredLength;
  T newTime = bps::BpsUtils<T, Dim>::getEventTimeFromMcmcState(state)
      + requiredLength;
  return bps::BpsUtils<T, Dim>::createBpsMcmcState(
      newLocation, velocity, newTime);
}

template<typename T>
T calculateVariance(const std::vector<T>& numbers) {
  T mean = (T) 0.0;
  for (const T& number : numbers) {
    mean += number;
  }
  mean /= numbers.size();

  T variance = (T) 0.0;
  for (const T& number : numbers) {
    variance += (number - mean) * (number - mean);
  }
  variance /= (numbers.size() - 1);

  return variance;
}

// Using batch means we estime the CLT for Markov Chains variance.
template<typename T, int Dim>
T estimateMarkovChainsCltVariance(
    int numberOfBatches,
    const typename bps::Mcmc<T, Dim>::SampleOutput& samples,
    const std::function<T(Eigen::Matrix<T, Dim, 1>)>& function) {

  T totalPathLength = bps::BpsUtils<T, Dim>::getTotalPathLength(samples);
  T batchLength = totalPathLength / (T) numberOfBatches;
  std::vector<T> batchMeans;

  auto leftEndpoint = samples[0];
  auto rightEndpoint = samples[1];
  int nextPiecewiseSegmentRightIndex = 2;
  int numberOfBatchesProcessed = 0;
  while (numberOfBatchesProcessed < numberOfBatches) {
    T batchMean = (T) 0.0;

    T currentBatchLength = (T) 0.0;
    while (batchLength - currentBatchLength
           > std::numeric_limits<T>::epsilon()) {

      T segmentLength = bps::BpsUtils<T, Dim>
        ::getTimeDifferenceBetweenBpsMcmcStates(leftEndpoint, rightEndpoint);

      if (currentBatchLength + segmentLength <= batchLength) {
        // We can integrate the whole segment.
        batchMean += bps::BpsUtils<T, Dim>::integrateAlongPiecewiseLinearSegment(
            leftEndpoint, rightEndpoint, function);

        if (nextPiecewiseSegmentRightIndex == samples.size()) {
          // We have integrated the last segment.
          break;
        }

        leftEndpoint = rightEndpoint;
        rightEndpoint = samples[nextPiecewiseSegmentRightIndex];
        nextPiecewiseSegmentRightIndex += 1;
        currentBatchLength += segmentLength;
      } else {
        // Only a part of the current segment needs to be integrated for
        // the current batch.
        T remainingLength = batchLength - currentBatchLength;
        auto midPoint = advancePiecewiseLinearSegment(
            leftEndpoint, remainingLength);
        batchMean += bps::BpsUtils<T, Dim>
            ::integrateAlongPiecewiseLinearSegment(
                leftEndpoint, midPoint, function);
        leftEndpoint = midPoint;
        currentBatchLength = batchLength;
      }
    }

    batchMeans.push_back(batchMean * sqrt(1 / batchLength));
    numberOfBatchesProcessed += 1;
  }

  return calculateVariance(batchMeans);
}

// Estimate the real variance.
template<typename T, int Dim>
T estimateRealVariance(
    const typename bps::Mcmc<T, Dim>::SampleOutput& samples,
    const std::function<T(Eigen::Matrix<T, Dim, 1>)>& function) {

  auto meansVector = bps::BpsExpectationEstimators<T, Dim>
      ::numericalIntegrationEstimator(samples, function);
  T mean = meansVector[meansVector.size() - 1];

  std::function<T(Eigen::Matrix<T, Dim, 1>)> squaredFunction =
      [&] (Eigen::Matrix<T, Dim, 1> vector) -> T {
          T value = function(vector);
          return value * value;
      };

  meansVector = bps::BpsExpectationEstimators<T, Dim>
      ::numericalIntegrationEstimator(samples, squaredFunction);
  T integratedSquaredFunction = meansVector[meansVector.size() - 1];

  return integratedSquaredFunction - mean * mean;
}

}

namespace bps {

template<typename T, int Dim>
T IntegratedAutocorrelationTime<T, Dim>::calculateIntegratedAutocorrelationTime(
    const typename Mcmc<T, Dim>::SampleOutput& samples,
    const std::function<T(Eigen::Matrix<T, Dim, 1>)>& function,
    const int numberOfBatches) {

  T cltVariance = estimateMarkovChainsCltVariance(
      numberOfBatches, samples, function);
  T realVariance = estimateRealVariance(samples, function);

  return cltVariance / realVariance;
}

}
