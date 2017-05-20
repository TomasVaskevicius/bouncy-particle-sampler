#pragma once

#include <vector>

#include "analysis/output_processors/sequential_processor_base.h"

namespace pdmp {
namespace analysis {

/**
 * A processor which collects all the samples into a std::vector.
 * Note that this processor is not intended to be used for long runs.
 * Mostly intended for debugging purposes.
 */
template<class Pdmp, class State>
class PathCollector : public SequentialProcessorBase<Pdmp, State> {

 public:

  std::vector<State> getCollectedStates();

 protected:

  virtual void processTwoSequentialResults(
    const IterationResult<State>& first,
    const IterationResult<State>& second) override final;

 private:

  std::vector<State> collectedStates_;

};

}
}

#include "path_collector.tcc"
