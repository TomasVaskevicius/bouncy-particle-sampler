#pragma once

template<typename T, int Dim>
typename GaussianDistributions<T, Dim>::EnergyGradient
GaussianDistributions<T, Dim>::getGaussianDistributionEnergyGradient(
  const Eigen::Matrix<T, Dim, 1>& mean,
  const Eigen::Matrix<T, Dim, Dim>& covarianceMatrix) {

  Eigen::Matrix<T, Dim, Dim> invertedCovarianceMatrix = covarianceMatrix.inverse();
  return [invertedCovarianceMatrix, mean]
         (Eigen::Matrix<T, Dim, 1> x) -> Eigen::Matrix<T, Dim, 1> {
                 return invertedCovarianceMatrix * (x - mean);
  };
}
