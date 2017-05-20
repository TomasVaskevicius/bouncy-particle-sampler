#pragma once

namespace pdmp {
namespace analysis {

template<class Pdmp, class State>
std::vector<State> PathCollector<Pdmp, State>::getCollectedStates() {
  return collectedStates_;
}


template<class Pdmp, class State>
void PathCollector<Pdmp, State>::processTwoSequentialResults(
  const IterationResult<State>& first,
  const IterationResult<State>& second) {

  if (collectedStates_.size() == 0) {
    collectedStates_.push_back(first.state);
  }
  collectedStates_.push_back(second.state);
}

}
}
