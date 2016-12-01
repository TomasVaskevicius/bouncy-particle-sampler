#pragma once

#include "core/random_number_sampler.h"

#include <memory>
#include <Eigen/Core>

namespace bps {

/**
 * This abstract lass will represent states in MCMC algorithms.
 *
 * Each state must contain a real valued vector which represents the number
 * sampled in that state.
 */
template<typename FloatingPointType, int Dimensionality>
class McmcState {

 public:

  virtual ~McmcState() {}

  /**
   * Returns the number (column vector) which is represented by the
   * current state.
   *
   * @return
   *   The column vector represented by this state.
   */
  virtual Eigen::Matrix<FloatingPointType, Dimensionality, 1>
      getVectorRepresentedByState() const = 0;

};

/**
 * This abstract class encapsulates the general form of all MCMC algorithms.
 * That is:
 *
 * Initialize x
 * repeat {
 *   Generate pseudorandom change to x
 *   Output x
 * }
 *
 * For more information see Chapter 1 in "Handbook of Markov Chain Monte Carlo"
 * (http://www.mcmchandbook.net/).
 */
template<typename FloatingPointType, int Dimensionality>
class Mcmc : public RandomNumberSampler<FloatingPointType, Dimensionality> {

 public:

  virtual ~Mcmc() {}

  /**
   * Generates the next MCMC state given the current state of the algorithm.
   *
   * @return
   *   The next generated mcmc state. It is generated using only the last
   *   state.
   */
  virtual std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      generateNextState() const = 0;

  Eigen::Matrix<FloatingPointType, Dimensionality, 1>
      getNextSample() override final;

 protected:

  // Constructor to be called by derived classes for initializing mcmc state.
  Mcmc(std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      initialState);

  // Generates an initial state for starting this algorithm. Can
  // only be called once and should be called from the constructor.
  virtual std::unique_ptr<McmcState<FloatingPointType, Dimensionality>>
      getInitialState() const = 0;

  std::unique_ptr<McmcState<FloatingPointType, Dimensionality>> lastState_;

};

}

#include "mcmc.tcc"
