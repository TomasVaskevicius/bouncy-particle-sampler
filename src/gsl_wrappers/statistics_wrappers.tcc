#pragma once

#include<algorithm>

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics.h>

namespace bps {

template<typename T>
void GslStatisticsWrappers<T>::calculateStatisticsForBoxPlots(
    const std::vector<T>& data,
    double& minimum,
    double& firstQuartile,
    double& median,
    double& thirdQuartile,
    double& maximum) {

  const T* dataArray = &data[0];
  double convertedArray[data.size()];
  std::copy(dataArray, dataArray + data.size(), convertedArray);

  gsl_sort(convertedArray, 1, data.size());

  minimum = convertedArray[0];
  firstQuartile = gsl_stats_quantile_from_sorted_data(
      convertedArray, 1, data.size(), 0.25);
  median = gsl_stats_median_from_sorted_data(convertedArray, 1, data.size());
  thirdQuartile = gsl_stats_quantile_from_sorted_data(
      convertedArray, 1, data.size(), 0.75);
  maximum = convertedArray[data.size() - 1];
}

}
