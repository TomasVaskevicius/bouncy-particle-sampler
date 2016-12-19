#pragma once

#include <vector>

namespace bps {

/**
 * GNU Scientific Library wrappers for statistical calculations.
 */
template<typename FloatingPointType>
class GslStatisticsWrappers {

 public:

  /**
   * Given a vector of real numbers, 1st, 2nd and 3rd quartiles as well
   * as maximum and minimum elements.
   */
  static void calculateStatisticsForBoxPlots(
      const std::vector<FloatingPointType>& data,
      double& minimum,
      double& firstQuartile,
      double& median,
      double& thirdQuartile,
      double& maximum);

};

}

#include "statistics_wrappers.tcc"
