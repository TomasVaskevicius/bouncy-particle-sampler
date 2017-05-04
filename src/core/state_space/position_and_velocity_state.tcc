#pragma once

#include <stdexcept>

namespace pdmp {

template<typename T, int Dim>
PositionAndVelocityState<T, Dim>::PositionAndVelocityState(
  RealVector<Dim / 2>  position, RealVector<Dim / 2> velocity)
  : position(position), velocity(velocity) {
}

template<typename T, int Dim>
bool operator==(
  const PositionAndVelocityState<T, Dim>& lhs,
  const PositionAndVelocityState<T, Dim>& rhs) {

  return lhs.position.isApprox(rhs.position)
         && lhs.velocity.isApprox(rhs.velocity);
}

template<typename T, int Dim>
T PositionAndVelocityState<T, Dim>::getElementAtIndex(int index) const {
  if (index < 0 || index >= Dim) {
    throw std::out_of_range("Element index " + std::to_string(index) + " is"
                            " out of range. Should be 0 <= index < " +
                            std::to_string(Dim) + ".");
  }

  if (index < Dim / 2) {
    return this->position(index);
  } else {
    return this->velocity(index - Dim / 2);
  }
}

template<typename T, int Dim>
typename PositionAndVelocityState<T, Dim>::DynamicRealVector
PositionAndVelocityState<T, Dim>::getSubvector(std::vector<int> ids) const {
  if (ids.size() <= 0 || ids.size() > Dim) {
    throw std::out_of_range("Subvector size needs to be between 1 and " +
                            std::to_string(Dim) + ".");
  }

  DynamicRealVector subVector(ids.size());
  for (int i = 0; i < ids.size(); i++) {
    subVector(i) = this->getElementAtIndex(ids[i]);
  }
  return subVector;
}


template<typename T, int Dim>
template<class VectorType>
PositionAndVelocityState<T, Dim>
PositionAndVelocityState<T, Dim>::constructStateWithModifiedVariables(
  std::vector<int> ids, VectorType modification) const {

  if (ids.size() != modification.size()) {
    throw std::logic_error("The number of ids to be modified should be equal "
                           "to the modification vector size.");
  }

  RealVector<Dim / 2> newPosition = this->position;
  RealVector<Dim / 2> newVelocity = this->velocity;
  for (int i = 0; i < ids.size(); i++) {
    if (ids[i] < Dim / 2) {
      newPosition(ids[i]) = modification[i];
    } else {
      newVelocity(ids[i] - Dim / 2) = modification[i];
    }
  }
  return PositionAndVelocityState<T, Dim>(newPosition, newVelocity);
}

}
