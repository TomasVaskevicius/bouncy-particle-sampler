#pragma once

#include <memory>
#include <stdexcept>

#include <Eigen/Core>

#include "mcmc/utils.h"
#include "mcmc/bps/reflection_kernel.h"

#include <stan/math/prim/scal.hpp>

namespace pdmp {
namespace mcmc {

namespace {

// To be used with the flip predetermined variable Markov kernel.
auto getIndependentFlippingStrategy(double rate) {
  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
  auto rng = getRng();
  auto refreshmentStrategy =
    [rng, rate] (const auto& state, const auto&, const auto&) mutable {
      return dependencies_graph::wrapPoissonProcessResult(
        stan::math::exponential_rng(rate, rng));
    };
  return refreshmentStrategy;
}

// To be used with the independent flipping strategy.
auto flipPredeterminedVariable = [] (const auto& state) {
  if (state.size() != 1) {
    throw std::runtime_error(
        "Flip predetermined variable lambda called on "
        "a state with size " + std::to_string(state.size()) + " but the size "
        "should be 1.");
  }
  decltype(state) flipped = -1.0 * state;
  return flipped;
};

// A kernel, which samples one dimension component and flips it.
template<class F>
auto getFlipKernel(const F& energyGradient) {
  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
  auto rng = getRng();
  auto flipKernel =
    [rng, energyGradient] (const auto& state) mutable {
      auto position = state.head(state.size() / 2);
      RealVector velocity = state.tail(state.size() / 2);
      auto gradient = energyGradient(position);

      // Sample which velocity components needs to be flipped.
      double gradientPositivePartSum = 0.0;
      for (int i = 0; i < gradient.size(); i++) {
        gradientPositivePartSum += std::max(
          0.0, (double) gradient(i) * (double) state(i));
      }
      double U = stan::math::uniform_rng(0.0, 1.0, rng);
      int flipIndex = 0;
      for (flipIndex = 0; flipIndex < gradient.size(); flipIndex++) {
        U -= (1.0 / gradientPositivePartSum) * std::max(
              0.0, (double) gradient(flipIndex) * (double) state(flipIndex));
        if (U <= 0) {
          break;
        }
      }

      velocity(flipIndex) = velocity(flipIndex) * (-1.0);
      RealVector returnVector(state.size());
      returnVector << position, velocity;
      return returnVector;
    };
  return flipKernel;
}

}

namespace zig_zag {

// In addition to the model variables (position) add the velocity variables,
static std::vector<int> getPositionAndVelocityVariables(
  const std::vector<int>& positionVariableIds, int modelDimension) {

  std::vector<int> allVariables = positionVariableIds;
  for (int i = 0; i < positionVariableIds.size(); i++) {
    allVariables.push_back(positionVariableIds[i] + modelDimension);
  }
  return allVariables;
}

// Return the vector with velocity variable ids for the given position
// variables.
static std::vector<int> getVelocityVariables(
  const std::vector<int>& positionVariableIds, int modelDimension) {

  std::vector<int> velocityVariables;
  for (int i = 0; i < positionVariableIds.size(); i++) {
    velocityVariables.push_back(positionVariableIds[i] + modelDimension);
  }
  return velocityVariables;
}

}

ZigZagBuilder::ZigZagBuilder(int numberOfModelVariables)
  : PdmpBuilderBase<bps::State, bps::Flow>(numberOfModelVariables * 2),
    numberOfModelVariables_(numberOfModelVariables) {

  // Add extra switching rates for each dimensional component.
  for (int i = 0; i < numberOfModelVariables; i++) {
    auto indepFlippingStrategy = getIndependentFlippingStrategy(
      1.0 / numberOfModelVariables);
    PdmpBuilderBase<bps::State, bps::Flow>::addFactorNode(
      std::vector<int>{}, indepFlippingStrategy);
    PdmpBuilderBase<bps::State, bps::Flow>::addMarkovKernelNode(
      std::vector<int>{i + numberOfModelVariables},
      std::vector<int>{i + numberOfModelVariables},
      flipPredeterminedVariable);
  }
}

template<class Distribution>
void ZigZagBuilder::addFactor(
    const std::vector<int>& variableIds,
    const DistributionBase<Distribution>& distribution) {

  const std::vector<int> variablesNeededByFlipKernel =
    zig_zag::getPositionAndVelocityVariables(
      variableIds, this->numberOfModelVariables_);
  const std::vector<int> variablesToBeChangedByFlipKernel =
    zig_zag::getVelocityVariables(variableIds, this->numberOfModelVariables_);
  const std::vector<int> variablesNeededByFactorNode =
    variablesNeededByFlipKernel;

  auto logPdf = distribution.getLogPdf();
  auto logProbGradient = getGradientOfAFunctor(logPdf);
  auto energy = [logProbGradient] (const auto& state) {
    auto logPrGrad = logProbGradient(state);
    decltype(logPrGrad) negated = logPrGrad * (-1.0);
    return negated;
  };
  auto flipKernel = getFlipKernel(energy);
  auto poissonProcessStrategy = distribution.template getPoissonProcessStrategy<
    zig_zag::Flow>();

  PdmpBuilderBase<bps::State, bps::Flow>::addFactorNode(
    variablesNeededByFactorNode, poissonProcessStrategy);
  PdmpBuilderBase<bps::State, bps::Flow>::addMarkovKernelNode(
    variablesNeededByFlipKernel,
    variablesToBeChangedByFlipKernel,
    flipKernel);

}

auto ZigZagBuilder::build() {
  return PdmpBuilderBase<bps::State, bps::Flow>::build();
}

}
}
