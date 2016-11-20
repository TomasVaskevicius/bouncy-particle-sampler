#pragma once

#include <Eigen/Core>
#include <random>
#include <type_traits>
#include <vector>

namespace bps {

/**
 * An abstract class for representing sampling algorithms.
 *
 * It is parameterized by
 *   1) FloatingPointType (such as float, double or long double) which
 * specifies the precision of the mathematical operations.
 *   2) Dimensionality, which must be a strictly positive integer, specifying
 * the dimensionality of the distribution we want to sample from.
 */
template<typename FloatingPointType, int Dimensionality>
class RandomNumberSampler {

  static_assert(std::is_floating_point<FloatingPointType>::value,
                "The fist parameter in all RandomNumberSampler subclasses must "
		"be of floating point type (i.e. float, double, long double).");
  static_assert(Dimensionality >= 1,
                "The dimensionality parameter in all RandomNumberSampler "
		"subclasses must be a strictly positive integer.");

 public:

  RandomNumberSampler();

  virtual ~RandomNumberSampler() {}

  /**
   * Returns the next sample from this random number sampling algorithm.
   * The method getNextSamples(int) is implemented using this method
   * as a subroutine.
   */
  virtual Eigen::Matrix<FloatingPointType, Dimensionality, 1>
      getNextSample() = 0;

  /**
   * Returns a std::vector of numbers sampled from the distribution represented
   * by an instance of this class. The next time this method is called, the
   * algorithm continues from the state left by the last call.
   *
   * @param numberOfIterations
   *   The number of samples which should be returned by this method.
   *   This parameter must be strictly positive.
   * @return
   *   an array of vectors which is the output of our sampling algorithm.
   */
  std::vector<Eigen::Matrix<FloatingPointType, Dimensionality, 1>>
      getNextSamples(int numberOfSamples);

 protected:

  std::default_random_engine rng_;

};

}

#include "core/random_number_sampler.tcc"
