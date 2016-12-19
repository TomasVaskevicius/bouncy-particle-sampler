#pragma once

#include "core/mcmc.h"

#include <string>
#include <vector>

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
   *
   * @param smapleRun
   *   Output of the BPS algorithm.
   */
  void plotTwoDimensionalSamplePath(
    const typename Mcmc<FloatingPointType, 2>::SampleOutput& sampleRun);

  /**
   * This method generated box plots.
   *
   * @param data
   *   A vector of data for each box.
   * @param names
   *   Names for each box. Size of this vector must be equal to the
   *   size of data vector.
   */
  void plotBoxPlot(
    const std::vector<std::vector<FloatingPointType>>& data,
    const std::vector<std::string>& names);

 private:

  QApplication qApplication_;
  std::unique_ptr<QMainWindow> qMainWindow_;

};

}
}

#include "analysis/plotting_utils.tcc"
