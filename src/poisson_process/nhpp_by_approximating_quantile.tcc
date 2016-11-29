#pragma once

#include <limits>

#include "gsl_wrappers/integration_wrappers.h"
#include "gsl_wrappers/root_finding_wrappers.h"

// TODO:
//
// 1) Implement and profile Newton-Rhapson method for root finding at points
//    where the derivative is non-zero.
// 2) Optimize the wrapped integration function, so that it stops recalculating
//    the same values.
// 3) Extract "error" constants such as 1e-7 to another file, so that numeric
//    errors are managed in unified way everywhere.

namespace {

// We will want to find a root of a function in some interval
// where the lower bound is L. Once searching for an upper bound,
// we will first try L + kFirstProposalTranslation;
const double kFirstProposalTranslation = 8.0;

// Once we have found a root and try to search for smaller roots,
// what is the first smaller value that we will try?
// Note that this translation value can't be too big, or else the
// binary search algorithm will take too long in cases when we
// have already found the best root.
const double kFirstSmallerRootPropostalTranslation = 1e-7;

// This helper function, given continuous function f and a lower bound value
// a such that f(a) <= 0.0, returns value b, such that f(b) >= 0.0.
// We also assume that such value b exists.
template<typename T>
T findUpperboundForRootFindingAlgorithm(
    std::function<T(T)> integralFunction, T lowerBound) {

  T proposedUppoerBound = lowerBound + (T) kFirstProposalTranslation;
  while (integralFunction(proposedUppoerBound) < 0.0) {
    proposedUppoerBound *= 2.0;
  }

  return proposedUppoerBound;
}

template<typename T>
bool areEqual(T a, T b) {
  T difference = a - b;
  T epsilon = std::numeric_limits<T>::epsilon();
  return difference < epsilon && -difference < epsilon;
}

template<typename T>
T tryToFindSmallestRoot(std::function<T(T)> integralFunction, T knownRoot) {
  T proposedSmallerRoot = knownRoot;
  T smallestKnownRoot = knownRoot;
  T epsilon =  (T) kFirstSmallerRootPropostalTranslation;
  T integralValueAtKnownRoot = integralFunction(knownRoot);

  // Find some value (proposedSmallerRoot) which is strictly smaller
  // than the infinum of roots.
  do {
    smallestKnownRoot = proposedSmallerRoot;
    proposedSmallerRoot -= epsilon;
    epsilon *= 2.0;
  } while (areEqual(integralValueAtKnownRoot,
                    integralFunction(proposedSmallerRoot)));

  // Use binary search to finish the search in interval
  // [proposedSmallerRoot, smallestKnownRoot].
  while (smallestKnownRoot - proposedSmallerRoot > 1e-7) {
    T midInterval = (smallestKnownRoot + proposedSmallerRoot) / 2.0;
    if (areEqual(integralValueAtKnownRoot,
                 integralFunction(midInterval))) {

      smallestKnownRoot = midInterval;
    } else {
      proposedSmallerRoot = midInterval;
    }
  }

  return smallestKnownRoot;
}

}

namespace bps {

template<typename T>
NonHomogeneousPoissonProcessByApproximatingQuantile<T>
    ::NonHomogeneousPoissonProcessByApproximatingQuantile(
        std::function<T(T)> intensity)
  : NonHomogeneousPoissonProcessByTimeScaling<T>(
        generateIntegratedIntensityQuantile(),
        intensity),
    integratedIntensityAtLastJumpTime_(0) {
}

template<typename T>
void NonHomogeneousPoissonProcessByApproximatingQuantile<T>::reset() {
  NonHomogeneousPoissonProcessByTimeScaling<T>::reset();
  this->integratedIntensityAtLastJumpTime_ = 0;
}

template<typename T>
std::function<T(T)> NonHomogeneousPoissonProcessByApproximatingQuantile<T>
    ::generateIntegratedIntensityQuantile() {

  return [this] (T targetIntegratedIntensityValue) -> T {
    T lastIntegratedIntensityValue = this->integratedIntensityAtLastJumpTime_;

    // In an unfortunate event, that our errors were too big and
    // targetIntegratedIntensityValue < lastIntegratedIntensityValue
    // we just add epsilon to last jump time.
    if (targetIntegratedIntensityValue < lastIntegratedIntensityValue) {
      return this->lastJumpTime_ + 1e-7;
    }

    std::function<T(T)> translatedIntensityIntegral =
        [&] (T x) -> T {
          T translateBy = lastIntegratedIntensityValue
                          - targetIntegratedIntensityValue;
          T integralAfterLastJump = bps::GslIntegrationWrappers<T>::integrate(
              this->intensity_, this->lastJumpTime_, x);

          return  translateBy + integralAfterLastJump;
         };

    T searchIntervalLowerBound = this->lastJumpTime_;
    T searchIntervalUpperBound = findUpperboundForRootFindingAlgorithm(
        translatedIntensityIntegral, searchIntervalLowerBound);

    T root = bps::GslRootFindingWrappers<T>::brentSolver(
                 translatedIntensityIntegral,
                 searchIntervalLowerBound,
                 searchIntervalUpperBound);

    // Since we are searching for {inf x | translatedIntensityIntegral(x) = 0}
    // we will still try to look for smaller value than root.
    T smallestRoot = tryToFindSmallestRoot(
        translatedIntensityIntegral, root);

    this ->integratedIntensityAtLastJumpTime_ = targetIntegratedIntensityValue;
    return smallestRoot;
  };
}

}
