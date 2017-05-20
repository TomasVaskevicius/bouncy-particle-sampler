#pragma once

#include <stdexcept>

#include <Eigen/Core>

namespace pdmp {
namespace mcmc {

template <class F>
auto getReflectionKernel(const F& logProbGradient) {
  using RealVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
  auto kernel = [logProbGradient] (auto stateVector) {
    if (stateVector.size() % 2 != 0) {
      throw std::runtime_error(
        "The BPS reflection kernel was invoked on a vector of odd size.");
    }
    RealVector position = stateVector.head(stateVector.size() / 2);
    RealVector velocity = stateVector.tail(stateVector.size() / 2);
    RealVector energyGradient = -1.0 * logProbGradient(position);
    auto reflectedVelocity =
      velocity - 2.0 * (energyGradient.dot(velocity) * energyGradient)
                       / energyGradient.squaredNorm();

    RealVector newVector(stateVector.size());
    newVector << position, reflectedVelocity;
    return newVector;
  };
  return kernel;
}

}
}
