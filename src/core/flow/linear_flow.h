#pragma once

namespace pdmp {

class LinearFlow {

 public:

  template<class State, typename RealType>
  static State advanceStateByFlow(State state, RealType time);

};

}

#include "linear_flow.tcc"
