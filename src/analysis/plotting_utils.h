#pragma once

#include "core/mcmc.h"

#include <QApplication>
#include <QMainWindow>

namespace bps {
namespace analysis {

/**
 * A helper class for visual analysis of
 */
template<typename FloatingPointType>
class PlottingUtils {

 public:

  /**
   * Default constructor if no command line arguments are passed.
   */
  PlottingUtils();

  /**
   * Command line arguments need to be passed to the constructor.
   */
  PlottingUtils(int& argc, char* argv[]);

  /**
   * Plots the sample path of two dimensional BPS output.
   */
  void plotTwoDimensionalSamplePath(
    const typename Mcmc<FloatingPointType, 2>::SampleOutput& sampleRun);

 private:

  QApplication qApplication_;
  std::unique_ptr<QMainWindow> qMainWindow_;

};

}
}

#include "analysis/plotting_utils.tcc"
