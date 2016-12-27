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
   * @param fileName
   *   The name of output image to save.
   *   If not set, will output the image on the screen.
   *
   */
  void plotTwoDimensionalSamplePath(
    const typename Mcmc<FloatingPointType, 2>::SampleOutput& sampleRun,
    const std::string& fileName = "");

  /**
   * This method generated box plots.
   *
   * @param data
   *   A vector of data for each box.
   * @param names
   *   Names for each box. Size of this vector must be equal to the
   *   size of data vector.
   * @param fileName
   *   The name of output image to save.
   *   If not set, will output the image on the screen.
   */
  void plotBoxPlot(
    const std::vector<std::vector<FloatingPointType>>& data,
    const std::vector<std::string>& names,
    const std::string& fileName = "");

  /**
   * Plots given lines on the same graph. The given vectors must be of
   * the same size and each pair of elemts (xAxisValues[i], yAxisValues[i])
   * must also be of the same size.
   *
   * @param xAxisVector
   *   For each line to plot, values for x axis.
   * @param yAxisVector
   *   For each line to plot, values for y axis.
   * @param xAxisName
   *   The x axis name.
   * @param yAxisName
   *   The y axis name.
   * @param fileName
   *   The name of output image to save.
   *   If not set, will output the image on the screen.
   */
  void plotLineGraphs(
    const std::vector<std::vector<FloatingPointType>>& xAxisValues,
    const std::vector<std::vector<FloatingPointType>>& yAxisValues,
    const std::string& xAxisName,
    const std::string& yAxisName,
    const std::string& fileName = "");

 private:

  // Shows the current plot on screen.
  void showImageOnScreen();

  // If fileName = "", outputs image on screen. Else, saves a .png image.
  void handleImageOutput(const std::string& fileName);

  QApplication qApplication_;
  std::unique_ptr<QMainWindow> qMainWindow_;

};

}
}

#include "analysis/plotting_utils.tcc"
