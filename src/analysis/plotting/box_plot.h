#pragma once

#include <vector>
#include <string>

namespace pdmp {
namespace analysis {

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
  const std::vector<std::vector<double>>& data,
  const std::vector<std::string>& names,
  const std::string& fileName = "");

}
}
