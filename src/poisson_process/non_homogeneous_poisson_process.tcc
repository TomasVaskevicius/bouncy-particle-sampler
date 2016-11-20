#pragma once

namespace bps {

template<typename T>
NonHomogeneousPoissonProcess<T>::NonHomogeneousPoissonProcess(
    std::function<T(T)> intensity)
  : intensity_(intensity) {
}

}
