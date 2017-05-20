#pragma once

namespace pdmp {
namespace mcmc {

template<class Derived>
auto DistributionBase<Derived>::getLogPdf() const {
  return static_cast<const Derived*>(this)->getLogPdf();
}


template<class Derived>
template<class Flow, class... Args>
auto DistributionBase<Derived>::getPoissonProcessStrategy(Args&&... args)
  const {

  return static_cast<const Derived*>(this)->
    template getPoissonProcessStrategy<Flow>(std::forward<Args>(args)...);
}

}
}

#include "distribution_base.tcc"
