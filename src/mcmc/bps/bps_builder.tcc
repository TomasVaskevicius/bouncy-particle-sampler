#pragma once

#include <memory>

#include <Eigen/Core>

#include "mcmc/utils.h"
#include "mcmc/bps/reflection_kernel.h"

#include <stan/math/prim/scal.hpp>

namespace pdmp {
namespace mcmc {

namespace {

auto getRefreshmentStrategy(double refreshRate) {
  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
  auto rng = getRng();
  auto refreshmentStrategy =
    [rng, refreshRate] (const auto& state, const auto&, const auto&) mutable {
      return dependencies_graph::wrapPoissonProcessResult(
        stan::math::exponential_rng(refreshRate, rng));
    };
  return refreshmentStrategy;
}

auto getRefreshmentKernel() {
  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
  auto rng = getRng();
  auto refreshmentKernel =
    [rng] (const auto& state) mutable {
      int stateSize = state.size();
      RealVector refreshed(stateSize);
      for (int i = 0; i < stateSize; i++) {
        refreshed(i) = stan::math::normal_rng(0.0, 1.0, rng);
      }
      return refreshed;
    };
  return refreshmentKernel;
}

}

namespace {

// In addition to the model variables (position) add the velocity variables,
std::vector<int> getPositionAndVelocityVariables(
  const std::vector<int>& positionVariableIds, int modelDimension) {

  std::vector<int> allVariables = positionVariableIds;
  for (int i = 0; i < positionVariableIds.size(); i++) {
    allVariables.push_back(positionVariableIds[i] + modelDimension);
  }
  return allVariables;
}

// Return the vector with velocity variable ids for the given position
// variables.
std::vector<int> getVelocityVariables(
  const std::vector<int>& positionVariableIds, int modelDimension) {

  std::vector<int> velocityVariables;
  for (int i = 0; i < positionVariableIds.size(); i++) {
    velocityVariables.push_back(positionVariableIds[i] + modelDimension);
  }
  return velocityVariables;
}

}

BpsBuilder::BpsBuilder(int numberOfModelVariables)
  : PdmpBuilderBase<bps::State, bps::Flow>(numberOfModelVariables * 2),
    numberOfModelVariables_(numberOfModelVariables) {
}

template<class Distribution>
void BpsBuilder::addFactor(
    const std::vector<int>& variableIds,
    const DistributionBase<Distribution>& distribution,
    double refreshRate) {

  const std::vector<int> variablesNeededByReflectionKernel =
    getPositionAndVelocityVariables(variableIds, this->numberOfModelVariables_);
  const std::vector<int> variablesToBeChangedByReflectionKernel =
    getVelocityVariables(variableIds, this->numberOfModelVariables_);
  const std::vector<int> variablesNeededByFactorNode =
    variablesNeededByReflectionKernel;

  auto logPdf = distribution.getLogPdf();
  auto logProbGradient = getGradientOfAFunctor(logPdf);
  auto reflectionKernel = getReflectionKernel(logProbGradient);
  auto poissonProcessStrategy = distribution.template getPoissonProcessStrategy<
    bps::Flow>();

  PdmpBuilderBase<bps::State, bps::Flow>::addFactorNode(
    variablesNeededByFactorNode, poissonProcessStrategy);
  PdmpBuilderBase<bps::State, bps::Flow>::addMarkovKernelNode(
    variablesNeededByReflectionKernel,
    variablesToBeChangedByReflectionKernel,
    reflectionKernel);

  // Now we need to add a the refreshment factor.
  // This could be further refactored into refreshment policy to allow
  // for more flexibility.
  const std::vector<int> variablesNeededByRefreshmentNode;
  const std::vector<int> variablesToBeChangedByRefreshmentKernel =
    variablesToBeChangedByReflectionKernel;

  auto refreshmentStrategy = getRefreshmentStrategy(refreshRate);
  auto refreshmentKernel = getRefreshmentKernel();

  PdmpBuilderBase<bps::State, bps::Flow>::addFactorNode(
    variablesNeededByRefreshmentNode, refreshmentStrategy);
  PdmpBuilderBase<bps::State, bps::Flow>::addMarkovKernelNode(
    variablesToBeChangedByRefreshmentKernel,
    variablesToBeChangedByRefreshmentKernel,
    refreshmentKernel);
}

auto BpsBuilder::build() {
  return PdmpBuilderBase<bps::State, bps::Flow>::build();
}

}
}
