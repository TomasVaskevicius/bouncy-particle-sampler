#pragma once

#include <gmock/gmock.h>

#include "core/pdmp.h"

/**
 * In this header file we define mock policies for piecewise-deterministic
 * Markov process simulation.
 */

// Forward declare mock classes.
struct MockPoissonProcess;
struct MockMarkovKernel;
struct MockFlow;

// Typedef the dummy Pdmp type to be used for unit testing.
typedef pdmp::Pdmp<MockPoissonProcess, MockMarkovKernel, MockFlow>
        DummyPdmp;

struct DummyState {
  using RealType = float;
  DummyState(int state) : state(state) {}
  int state;
};

bool operator==(const DummyState& lhs, const DummyState& rhs) {
  return lhs.state == rhs.state;
}

struct MockPoissonProcess {
  MockPoissonProcess() : MockPoissonProcess(0) {};
  MockPoissonProcess(int dummyVariable)
    : dummyPoissonProcessVariable(dummyVariable) {};
  const int dummyPoissonProcessVariable;
  MOCK_METHOD2(getJumpTime, float(const DummyState&, const DummyPdmp&));
};

struct MockMarkovKernel {
  MockMarkovKernel() : MockMarkovKernel(0) {};
  MockMarkovKernel(int dummyVariable)
    : dummyMarkovKernelVariable(dummyVariable) {};
  const int dummyMarkovKernelVariable;
  MOCK_METHOD2(jump, DummyState(const DummyState&, const DummyPdmp&));
};

struct MockFlow {
  MockFlow() : MockFlow(0) {};
  MockFlow(int dummyVariable) : dummyFlowVariable(dummyVariable) {};
  const int dummyFlowVariable;
  MOCK_METHOD2(advanceStateByFlow, DummyState(const DummyState&, float));
};
