#pragma once

namespace bps {

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> Mcmc<T, Dim>::getNextSample() {
  auto nextState = this->generateNextState();
  this->lastState_ = std::move(nextState);
  return (this->lastState_)->getVectorRepresentedByState();
}

template<typename T, int Dim>
Mcmc<T, Dim>::Mcmc(std::unique_ptr<McmcState<T, Dim>> initialState)
  : lastState_(std::move(initialState)) {
}

}
