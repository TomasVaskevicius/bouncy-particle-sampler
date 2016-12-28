#pragma once

#include "gsl_wrappers/statistics_wrappers.h"

#include <algorithm>

#include <qcustomplot.h>
#include <QDir>

namespace {

void createRequiredDirectories(const QString& filePath) {
  QFileInfo fileInfo(filePath);
  QDir fileDir = fileInfo.dir();
  QDir currentDir;
  currentDir.mkpath(fileDir.absolutePath());
}

template<typename T>
QVector<double> convertVectorToQVector(const std::vector<T>& stdVector) {
  QVector<double> qVector;
  for (const auto& value : stdVector) {
    qVector.push_back((double) value);
  }
  return qVector;
}

// We consider datapoints outliers if one of the following inequalities hold:
// datapoint >= thirdQuartile * 1.5 or datapoint <= firstQuartile / 1.5
template<typename T>
QVector<double> getOutliers(
  const std::vector<T>& data,
  const double& firstQuartile,
  const double& thirdQuartile) {

  QVector<double> outliers;
  for (const T& datapoint : data) {
    if (datapoint >= thirdQuartile * 1.5 || datapoint <= firstQuartile / 1.5) {
      outliers.push_back(datapoint);
    }
  }
  return outliers;
}

}

namespace bps {
namespace analysis {

template<typename T>
PlottingUtils<T>::PlottingUtils()
    : PlottingUtils(*(new int(1)), new char* [1]) {
}

template<typename T>
PlottingUtils<T>::PlottingUtils(int& argc, char* argv[])
    : qApplication_(argc, argv),
      qMainWindow_(new QMainWindow()) {
}

template<typename T>
void PlottingUtils<T>::plotTwoDimensionalSamplePath(
    const typename Mcmc<T, 2>::SampleOutput& sampleRun,
    const std::string& fileName) {

  QCustomPlot* customPlot = new QCustomPlot();
  this->qMainWindow_->setCentralWidget(customPlot);

  QVector<double> x, y;

  for (const auto& sample : sampleRun) {
    auto location = sample->getVectorRepresentedByState();
    x.push_back(location(0));
    y.push_back(location(1));
  }

  QCPCurve* samplePath = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
  samplePath->setData(x, y);

  customPlot->addGraph();
  customPlot->xAxis->rescale();
  customPlot->yAxis->rescale();
  customPlot->yAxis->setScaleRatio(customPlot->xAxis, 1.0);
  customPlot->replot();

  this->handleImageOutput(fileName);
}

template<typename T>
void PlottingUtils<T>::plotBoxPlot(
    const std::vector<std::vector<T>>& data,
    const std::vector<std::string>& names,
    const std::string& fileName) {

  QCustomPlot* customPlot = new QCustomPlot();
  this->qMainWindow_->setCentralWidget(customPlot);

  QCPStatisticalBox* boxPlot = new QCPStatisticalBox(
      customPlot->xAxis, customPlot->yAxis);
  QBrush boxBrush(QColor(60, 60, 255, 100));
  boxBrush.setStyle(Qt::Dense6Pattern);
  boxPlot->setBrush(boxBrush);

  QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

  double yAxisMinimum = data[0][0], yAxisMaximum = data[0][0];
  for (int i = 0; i < data.size(); i++) {
    // Add name for the i-th box.
    textTicker->addTick(i, QString::fromStdString(names[i]));

    // Calculate box plot statistics for i-th vector of data points.
    double min, firstQuartile, median, thirdQuartile, max;
    GslStatisticsWrappers<T>::calculateStatisticsForBoxPlots(
        data[i], min, firstQuartile, median, thirdQuartile, max);
    QVector<double> outliers = getOutliers(
      data[i], firstQuartile, thirdQuartile);

    double whiskerMax = std::min(thirdQuartile * 1.5, max);
    double whiskerMin = std::max(firstQuartile / 1.5, min);

    boxPlot->addData(
      i,
      whiskerMin,
      firstQuartile,
      median,
      thirdQuartile,
      whiskerMax,
      outliers);

    // Update minimum and maximum values found so far.
    yAxisMinimum = (min < yAxisMinimum) ? min : yAxisMinimum;
    yAxisMaximum = (max > yAxisMaximum) ? max : yAxisMaximum;
  }

  // Add some margin above and below the boxes.
  double boxPlotMargin = (yAxisMaximum - yAxisMinimum) / 15.0;
  yAxisMaximum += boxPlotMargin;
  yAxisMinimum = (yAxisMinimum <= 0.0 || yAxisMinimum - boxPlotMargin >= 0.0) ?
                  yAxisMinimum - boxPlotMargin : 0.0;

  customPlot->addGraph();
  customPlot->rescaleAxes();

  customPlot->yAxis->setRange(yAxisMinimum, yAxisMaximum);

  customPlot->xAxis->setSubTicks(false);
  customPlot->xAxis->setTicker(textTicker);
  customPlot->xAxis->scaleRange(1.25, customPlot->xAxis->range().center());

  customPlot->replot();

  this->handleImageOutput(fileName);
}

template<typename T>
void PlottingUtils<T>::plotLineGraphs(
    const std::vector<std::vector<T>>& xAxisValues,
    const std::vector<std::vector<T>>& yAxisValues,
    const std::string& xAxisName,
    const std::string& yAxisName,
    const std::string& fileName) {

  QCustomPlot* customPlot = new QCustomPlot();
  this->qMainWindow_->setCentralWidget(customPlot);

  for (int i = 0; i < xAxisValues.size(); i++) {
    auto x = convertVectorToQVector(xAxisValues[i]);
    auto y = convertVectorToQVector(yAxisValues[i]);
    customPlot->addGraph();
    customPlot->graph()->setData(x, y);
  }

  customPlot->xAxis->setLabel(QString::fromStdString(xAxisName));
  customPlot->yAxis->setLabel(QString::fromStdString(yAxisName));

  customPlot->rescaleAxes();
  customPlot->replot();

  this->handleImageOutput(fileName);
}

template<typename T>
void PlottingUtils<T>::showImageOnScreen() {
  this->qMainWindow_->setGeometry(100, 100, 500, 500);
  this->qMainWindow_->show();
  this->qApplication_.exec();
}

template<typename T>
void PlottingUtils<T>::handleImageOutput(const std::string& fileName) {
  if (fileName == "") {
    this->showImageOnScreen();
  } else {
    QString filePath = QString::fromStdString(fileName + ".png");
    createRequiredDirectories(filePath);
    QCustomPlot* customPlot = static_cast<QCustomPlot*>(
      this->qMainWindow_->centralWidget());
    customPlot->savePng(filePath, 800, 800);
  }
}

}
}
