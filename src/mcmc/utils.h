#pragma once

#include <random>

namespace pdmp {
namespace mcmc {

/**
 * A factory method for generating random engines.
 */
std::mt19937_64 getRng();

/**
 * Returns a gradient functor of a given functor.
 */
template<class F>
auto getGradientOfAFunctor(const F& functor);

}
}

#include "utils.tcc"
