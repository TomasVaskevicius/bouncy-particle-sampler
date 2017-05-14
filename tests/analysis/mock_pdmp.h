#pragma once

#include <gmock/gmock.h>

#include "core/pdmp.h"

struct DummyState {
  using RealType = float;
  DummyState(int id) : id(id) {}
  int id;
};

bool operator==(const DummyState& lhs, const DummyState& rhs) {
  return lhs.id == rhs.id;
}

struct MockPdmp {
  MOCK_METHOD1(
    simulateOneIteration, pdmp::IterationResult<DummyState>(const DummyState&));
};
