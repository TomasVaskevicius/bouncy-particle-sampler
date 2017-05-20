#pragma once

namespace pdmp {
namespace mcmc {

/**
 * Returns the BPS reflection kernel associated with the gradient of some
 * log probability density function
 * (not the energy, the energy is defined as the gradient of -log density).
 */
template <class F>
auto getReflectionKernel(const F& logProbGradient);

}
}

#include "reflection_kernel.tcc"
