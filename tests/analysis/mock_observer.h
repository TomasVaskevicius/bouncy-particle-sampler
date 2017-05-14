#pragma once

#include <gmock/gmock.h>

#include "analysis/pdmp_runner.h"
#include "core/pdmp.h"

#include "mock_pdmp.h"

struct MockObserver
  : public pdmp::analysis::ObserverBase<MockPdmp, DummyState> {

  MOCK_METHOD2(notifyProcessBegins, void(const MockPdmp&, const DummyState&));

  MOCK_METHOD1(
    notifyIterationResult, void(const pdmp::IterationResult<DummyState>&));

  MOCK_METHOD0(notifyProcessEnded, void());
};
