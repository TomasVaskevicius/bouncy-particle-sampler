#pragma once

namespace bps {

template<typename T, int Dim>
Mcmc<T, Dim>::Mcmc() {
}

template<typename T, int Dim>
Eigen::Matrix<T, Dim, 1> Mcmc<T, Dim>::getNextSample() {
  this->lastState_ = this->generateNextState();
  return (this->lastState_)->getVectorRepresentedByState();
}

template<typename T, int Dim>
std::vector<std::shared_ptr<McmcState<T, Dim>>>
    Mcmc<T, Dim>::getBatchOfMcmcStates(int batchSize) {

  std::vector<std::shared_ptr<McmcState<T, Dim>>> batch;
  for (int i = 0; i < batchSize; i++) {
    batch.push_back(this->lastState_);
    this->lastState_ = this->generateNextState();
  }
  return batch;
}

template<typename T, int Dim>
void Mcmc<T, Dim>::setCurrentState(std::shared_ptr<McmcState<T, Dim>>& state) {
  this->lastState_ = state;
}

template<typename T, int Dim>
void Mcmc<T, Dim>::reset() {
  this->lastState_ = this->getInitialState();
}

}
