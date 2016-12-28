#pragma once

#include "analysis/utils.h"

#include <iostream>

// TODO:
//
// 1) Start using logger instead of std::cout.
//

namespace {

const std::string kDefaultAlgorithmNamePrefix = "Algorithm";
const std::string kDefaultShortAlgorithmNamePrefix = "Alg";

// A helper for returning a vector of numbered names.
// Used by constructor to create default names for the algorithms.
std::vector<std::string> generateNames(std::string prefix, int count) {
  std::vector<std::string> names;
  for (int i = 1; i <= count; i++) {
    names.push_back(prefix + std::to_string(i));
  }
  return names;
}

// Given the benchmark name, generated the output directory name.
std::string generateOutputDirectoryName(std::string benchmarkName) {
  return "benchmarks/" + benchmarkName + "/";
}

template<typename T, int Dim>
void printTimeEstimateForGeneratingSamples(
  const typename bps::analysis::BpsBenchmark<T, Dim>::BpsFactoriesVector&
    bpsFactories,
  const T& requiredTrajectoryLengths,
  const int& numberOfRunsForEachAlgorithm,
  const int& numberOfCores) {

  T trajectoriesFraction = 1000.0;
  T timeEstimateInMilliseconds =
    bps::analysis::AnalysisUtils::getExecutionTime(
      [&] () -> void {
        for (int i = 0; i < bpsFactories.size(); i++) {
          bps::analysis::ParallelMcmcRunner<T, Dim>::generateBpsSamples(
            bpsFactories[i],
            requiredTrajectoryLengths / trajectoriesFraction,
            numberOfRunsForEachAlgorithm,
            numberOfCores);
         }
      }) * trajectoriesFraction;


  std::cout << "Estimated time for generatig samples is: ";
  if (timeEstimateInMilliseconds / 1000.0 / 3600.0 >= 1.0) {
    int hours = timeEstimateInMilliseconds / 1000.0 / 3600.0;
    int minutes = (int) timeEstimateInMilliseconds % (1000 * 3600);
    minutes /= 1000 * 60;
    std::cout << hours << " h " << minutes << " m." << std::endl;
  } else if (timeEstimateInMilliseconds / 1000.0 / 60.0 >= 1.0) {
    int minutes = timeEstimateInMilliseconds / 1000.0 / 60.0;
    int seconds = (int) timeEstimateInMilliseconds % 1000;
    std::cout << minutes << " m " << seconds << " s." << std::endl;
  } else {
    std::cout << timeEstimateInMilliseconds / 1000.0 << " s." << std::endl;
  }
}

}

namespace bps {
namespace analysis {

template<typename T, int Dim>
BpsBenchmark<T, Dim>::BpsBenchmark(
  const BpsFactoriesVector& bpsFactories,
  const std::vector<std::string>& algorithmNames,
  const std::vector<std::string>& shortAlgorithmNames,
  const std::string& benchmarkName)

  : bpsFactories_(bpsFactories),
    algorithmNames_(algorithmNames),
    shortAlgorithmNames_(shortAlgorithmNames),
    sampleOutputs_(nullptr),
    outputDirectory_(generateOutputDirectoryName(benchmarkName)) {
}


template<typename T, int Dim>
BpsBenchmark<T, Dim>::BpsBenchmark(
  const BpsFactoriesVector& bpsFactories,
  const std::string& benchmarkName)

  : BpsBenchmark(
      bpsFactories,
      generateNames(kDefaultAlgorithmNamePrefix, bpsFactories.size()),
      generateNames(kDefaultShortAlgorithmNamePrefix, bpsFactories.size()),
      benchmarkName) {
}

template<typename T, int Dim>
void BpsBenchmark<T, Dim>::generateSamples(
  const T& requiredTrajectoryLengths,
  const int& numberOfRunsForEachAlgorithm,
  const int& numberOfCores) {

  printTimeEstimateForGeneratingSamples<T, Dim>(
    this->bpsFactories_,
    requiredTrajectoryLengths,
    numberOfRunsForEachAlgorithm,
    numberOfCores);


  this->sampleOutputs_.reset(new std::vector<SampleOutputsVector>);
  this->runningTimes_.reset(new std::vector<std::vector<double>>);

  for (int i = 0; i < this->bpsFactories_.size(); i++) {
    std::vector<double> runningTimesForCurrentAlgorithm;

    this->sampleOutputs_->push_back(
      ParallelMcmcRunner<T, Dim>::generateBpsSamples(
        bpsFactories_[i],
        requiredTrajectoryLengths,
        numberOfRunsForEachAlgorithm,
        numberOfCores,
        &runningTimesForCurrentAlgorithm));

    this->runningTimes_->push_back(runningTimesForCurrentAlgorithm);
  }
}

template<typename T, int Dim>
void BpsBenchmark<T, Dim>::runBenchmark(
  const ExpectationEstimator& expectationEstimator,
  const RealFunctionOnSamples& realFunctionOnSamples,
  const std::string& benchmarkName,
  const int& numberOfBatchesForBatchMeans,
  const int& acfLagUpperbound,
  const int& lagStepSize) {

  std::string outputDir = this->outputDirectory_ + benchmarkName + "/";

  std::vector<std::vector<T>> iacts =
    this->outputIactBoxPlot(
      outputDir,
      expectationEstimator,
      realFunctionOnSamples,
      numberOfBatchesForBatchMeans);

  this->outputEffectiveSampleSizesPerSecond(outputDir, iacts);

  for (int i = 0; i < this->sampleOutputs_->size(); i++) {
    std::string outputDirForThisAlgorithm =
      outputDir + this->shortAlgorithmNames_[i] + "/";

    this->outputErrorsForSampleRuns(
      i,
      outputDirForThisAlgorithm,
      expectationEstimator,
      realFunctionOnSamples);

    this->outputAutocorrelationFunctions(
      i,
      outputDirForThisAlgorithm,
      expectationEstimator,
      realFunctionOnSamples,
      acfLagUpperbound,
      lagStepSize);

    if (Dim == 2) {
      this->plotTwoDimensionalSamplePaths(
        i,
        outputDirForThisAlgorithm + "sample_runs/");
    }
  }
}

template<typename T, int Dim>
void BpsBenchmark<T, Dim>::outputErrorsForSampleRuns(
  const int& algorithmId,
  const std::string& outputDir,
  const ExpectationEstimator& expectationEstimator,
  const RealFunctionOnSamples& realFunctionOnSamples) {

  auto sampleRuns = this->sampleOutputs_->at(algorithmId);

  auto meanEstimates = OutputAnalysis<T, Dim>::calculateExpectationEstimates(
    *sampleRuns,
    realFunctionOnSamples,
    expectationEstimator);

  auto eventTimes = OutputAnalysis<T, Dim>::getEventTimes(*sampleRuns);

  this->plottingUtils_.plotLineGraphs(
    eventTimes,
    meanEstimates,
    "Trajectory length",
    "Mean estimate",
    outputDir + "mean_estimates");
}

template<typename T, int Dim>
std::vector<std::vector<T>> BpsBenchmark<T, Dim>::outputIactBoxPlot(
  const std::string& outputDir,
  const ExpectationEstimator& expectationEstimator,
  const RealFunctionOnSamples& realFunctionOnSamples,
  const int& numberOfBatches) {

  std::vector<std::vector<T>> iacts;
  for (const auto& sampleRuns : *this->sampleOutputs_) {
    iacts.push_back(
      OutputAnalysis<T, Dim>::calculateIntegratedAutocorrelationTime(
        *sampleRuns,
        realFunctionOnSamples,
        expectationEstimator,
        70));
  }

  this->plottingUtils_.plotBoxPlot(
    iacts,
    this->shortAlgorithmNames_,
    outputDir + "iact");

  return iacts;
}

template<typename T, int Dim>
void BpsBenchmark<T, Dim>::outputEffectiveSampleSizesPerSecond(
  const std::string& outputDir,
  const std::vector<std::vector<T>>& iacts) {

  std::vector<std::vector<T>> essPerSecond;
  for (int i = 0; i < this->sampleOutputs_->size(); i++) {
    essPerSecond.push_back(
      OutputAnalysis<T, Dim>::calculateEffectiveSampleSizePerSecond(
        *(this->sampleOutputs_->at(i)),
        iacts[i],
        this->runningTimes_->at(i)));
  }

  this->plottingUtils_.plotBoxPlot(
    essPerSecond,
    this->shortAlgorithmNames_,
    outputDir + "essPerSecond");
}

template<typename T, int Dim>
void BpsBenchmark<T, Dim>::outputAutocorrelationFunctions(
  const int& algorithmId,
  const std::string& outputDir,
  const ExpectationEstimator& expectationEstimator,
  const RealFunctionOnSamples& realFunctionOnSamples,
  const int& acfLagUpperbound,
  const int& lagStepSize) {

  auto sampleRuns = this->sampleOutputs_->at(algorithmId);

  auto autocorrelationFunctions = OutputAnalysis<T, Dim>
    ::calculateAutocorrelationFunction(
      *sampleRuns,
      realFunctionOnSamples,
      expectationEstimator,
      acfLagUpperbound,
      lagStepSize);

  std::vector<double> lagTimes;
  for (int i = 0; i <= acfLagUpperbound; i += lagStepSize) {
    lagTimes.push_back(i);
  }

  std::vector<std::vector<double>> lagsForEachSampleRun(
    sampleRuns->size(), lagTimes);

  this->plottingUtils_.plotLineGraphs(
    lagsForEachSampleRun,
    autocorrelationFunctions,
    "Lag",
    "Autocorrelation",
    outputDir + "autocorrelation_functions");
}

template<typename T, int Dim>
void BpsBenchmark<T, Dim>::plotTwoDimensionalSamplePaths(
  const int& algorithmId,
  const std::string& outputDir) {

  if (Dim != 2) {
    return;
  }

  auto sampleRuns = this->sampleOutputs_->at(algorithmId);

  for (int i = 0; i < sampleRuns->size(); i++) {
    this->plottingUtils_.plotTwoDimensionalSamplePath(
      sampleRuns->at(i),
      outputDir + "run" + std::to_string(i + 1));
  }
}

}
}
