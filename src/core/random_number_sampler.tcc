#pragma once

#include <chrono>

namespace bps {

template<typename T, int Dim>
RandomNumberSampler<T, Dim>::RandomNumberSampler() {
  int seed = std::chrono::system_clock::now().time_since_epoch().count();
  rng_ = std::default_random_engine(seed);
}

template<typename T, int Dim>
std::vector<Eigen::Matrix<T, Dim, 1>> RandomNumberSampler<T, Dim>
    ::getNextSamples(int numberOfSamples) {

  std::vector<Eigen::Matrix<T, Dim, 1>> samples;
  for (int i = 0; i < numberOfSamples; i++) {
    Eigen::Matrix<T, Dim, 1> nextSample = this->getNextSample();
    samples.push_back(nextSample);
  }
  return samples;
}

}
