#pragma once

#include <iostream>

#include "analysis/pdmp_runner.h"

namespace pdmp {
namespace analysis {

/**
 * This output processor efficiently computes variances for each dimension.
 * Assumes that the linear flow was used, so that no numerical integration
 * needs to be performed.
 * Also assumes that the state space has a position member, which holds our
 * model variables.
 */
template<class Pdmp, class State>
class VarianceProcessor : public ObserverBase<Pdmp, State> {

 public:

  virtual void notifyProcessBegins(
    const Pdmp& pdmp, const State& initialState) override final {

    this->pdmp_ = &pdmp;
    for (int i = 0; i < initialState.position.size(); i++) {
      this->lastPositions_.push_back(initialState.position(i));
      this->lastVelocities_.push_back(initialState.velocity(i));
      this->trajectoryLengthsAtLastValues_.push_back(0.0);
      this->pathIntegrals_.push_back(0.0);
      this->pathSquaredIntegrals_.push_back(0.0);
    }
  }

  virtual void notifyIterationResult(
    const IterationResult<State>& iterationResult) override final {

    this->trajectoryLength_ += iterationResult.iterationTime;
    std::vector<int> changedVariables = this->pdmp_->getLastModifiedVariables();
    for (const int& variableId : changedVariables) {
      // The markov kernel modifies only velocity variables, so we need
      // to subtract the dimension from them.
      updateResults(
        variableId - this->lastPositions_.size(),
        iterationResult.state);
    }
  }

  virtual void notifyProcessEnded() override final {
    std::cout << "Total trajectory length generated: "
               << this->trajectoryLength_ << std::endl;
    // Update calculations for all the variables.
    for (int i = 0; i < lastPositions_.size(); i++) {
      double x = this->lastPositions_.at(i);
      double v = this->lastVelocities_.at(i);
      double t =
        this->trajectoryLength_ - this->trajectoryLengthsAtLastValues_.at(i);
      updateIntegralValues(i, x, v, t);
    }
  }

  void updateResults(int id, const State& currentState) {
    // Claculate the time difference between now and the last time we updated
    // our calculations for this variable.
    double t
      = this->trajectoryLength_ - this->trajectoryLengthsAtLastValues_.at(id);
    this->trajectoryLengthsAtLastValues_.at(id) = this->trajectoryLength_;

    double x = this->lastPositions_.at(id);
    double v = this->lastVelocities_.at(id);
    this->lastPositions_.at(id) = currentState.position(id);
    this->lastVelocities_.at(id) = currentState.velocity(id);

    updateIntegralValues(id, x, v, t);
  }

  void updateIntegralValues(int id, double x, double v, double t) {
    this->pathIntegrals_.at(id) += x * t + v * t * t / 2.0;
    this->pathSquaredIntegrals_.at(id) +=
      x * x * t + x * v * t * t + v * v * t * t * t / 3.0;
  }

  /**
   * Returns a variable estimate of a variable with the specified id.
   */
  double getVarianceEstimate(int id) const {
    double mean = this->pathIntegrals_.at(id) / this->trajectoryLength_;
    double squaredVariableMean =
      this->pathSquaredIntegrals_.at(id) / this->trajectoryLength_;
    return squaredVariableMean - mean * mean;
  }

 private:

  // We store our pdmp object, to look up which dimension components need to
  // be updated.
  const Pdmp* pdmp_;

  // We record what was the last state per dimension during the last
  // calculation..
  std::vector<double> lastPositions_;
  std::vector<double> lastVelocities_;

  // What was the trajectory length, when the last value (above) was recorded?
  std::vector<double> trajectoryLengthsAtLastValues_;

  // For each variable, the integrated path.
  std::vector<double> pathIntegrals_;

  // For each variable, the integrated path of squared value.
  std::vector<double> pathSquaredIntegrals_;

  // Total trajectory length so far.
  double trajectoryLength_{0.0};

};

}
}
