#pragma once

#include <vector>
#include <string>

namespace pdmp {
namespace analysis {

/**
 * Plots a two-dimensional piecewise linear sample path.
 *
 * @param x
 *   The x coordinates of the segment corners to be plotted.
 *   Must have the same number of elements as the y vector.
 * @param y
 *   The y coordinates of the segment corners to be plotted.
 *   Must have the same number of elements as the x vector.
 * @param fileName
 *   The name of output file, where the plot will be saved.
 *   If left empty, the plot will be displayed on screen.
 */
void plotTwoDimensionalPiecewiseLinearSamplePath(
  const std::vector<double>& x,
  const std::vector<double>& y,
  const std::string& fileName = "");

}
}
