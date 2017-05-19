#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "core/policies/linear_flow.h"

namespace pdmp {
namespace dependencies_graph {

namespace {

auto noOpIntensity = [] (auto, const auto&) -> double {
  throw std::runtime_error("Called unimplemented intensity method.");
};

}

// Forward declaration for the base class of Poisson process result;
struct PoissonProcessResultBase;
using PoissonProcessResultPtr = std::shared_ptr<PoissonProcessResultBase>;

template<class State>
struct FactorNodeBase {
  using RealType = typename State::RealType;
  FactorNodeBase(const std::vector<int>& dependentVariableIds);
  ~FactorNodeBase() = default;
  virtual RealType evaluateIntensity(
    const State& state, const RealType& time) = 0;
  virtual PoissonProcessResultPtr getPoissonProcessResult(
    const State& state) = 0;
  const std::vector<int> dependentVariableIds;
};

/**
 * A class for representing nodes associated to intensity factors.
 */
template<
  class State,
  class PoissonProcessLambda,
  class Flow = LinearFlow,
  class IntensityLambda = decltype(noOpIntensity)>
class FactorNode : public FactorNodeBase<State> {

 public:

  using RealType = typename FactorNodeBase<State>::RealType;
  static const int kStateSpaceDim = State::kStateSpaceDim;

  FactorNode(
    const std::vector<int>& dependentVariableIds,
    const PoissonProcessLambda& poissonProcessLambda,
    const IntensityLambda& intensityLambda = noOpIntensity);

  /**
   *  Evaluates the indensity at this after advancing the state
   *  by the flow with a specified amount of time.
   */
  virtual RealType evaluateIntensity(
    const State& state, const RealType& time = 0) override final;

  /**
   * Get Poisson process simulation result for a given state.
   */
  virtual PoissonProcessResultPtr
    getPoissonProcessResult(const State& state) override final;

 private:

  PoissonProcessLambda poissonProcessLambda_;
  IntensityLambda intensityLambda_;

};

}
}

#include "factor_node.tcc"
