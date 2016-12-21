#pragma once

#include "gsl_wrappers/statistics_wrappers.h"

#include <qcustomplot.h>

namespace bps {
namespace analysis {

template<typename T>
PlottingUtils<T>::PlottingUtils()
    : PlottingUtils(*(new int(1)), new char* [1]) {
}

template<typename T>
PlottingUtils<T>::PlottingUtils(int& argc, char* argv[])
    : qApplication_(argc, argv),
      qMainWindow_(new QMainWindow()){

  qMainWindow_->setCentralWidget(new QCustomPlot());
}

template<typename T>
void PlottingUtils<T>::plotTwoDimensionalSamplePath(
    const typename Mcmc<T, 2>::SampleOutput& sampleRun) {

  QCustomPlot* customPlot = static_cast<QCustomPlot*>(
      qMainWindow_->centralWidget());

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

  this->qMainWindow_->setGeometry(100, 100, 500, 500);
  this->qMainWindow_->show();

  this->qApplication_.exec();

  customPlot->clearPlottables();
}

template<typename T>
void PlottingUtils<T>::plotBoxPlot(
    const std::vector<std::vector<T>>& data,
    const std::vector<std::string>& names) {

  QCustomPlot* customPlot = static_cast<QCustomPlot*>(
      qMainWindow_->centralWidget());

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
    boxPlot->addData(i, min, firstQuartile, median, thirdQuartile, max);

    // Update minimum and maximum values found so far.
    yAxisMinimum = (min < yAxisMinimum) ? min : yAxisMinimum;
    yAxisMaximum = (max > yAxisMaximum) ? max : yAxisMaximum;
  }

  // Add some margin above and below the boxes.
  double boxPlotMargin = (yAxisMaximum - yAxisMinimum) / 8.0;
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

  this->qMainWindow_->setGeometry(100, 100, 500, 500);
  this->qMainWindow_->show();

  this->qApplication_.exec();

  customPlot->clearPlottables();
}

template<typename T>
void PlottingUtils<T>::plotLineGraphs(
    const std::vector<std::vector<T>>& xAxisValues,
    const std::vector<std::vector<T>>& yAxisValues,
    const std::string& xAxisName,
    const std::string& yAxisName) {

  QCustomPlot* customPlot = static_cast<QCustomPlot*>(
      qMainWindow_->centralWidget());

  for (int i = 0; i < xAxisValues.size(); i++) {
    auto x = this->convertVectorToQVector(xAxisValues[i]);
    auto y = this->convertVectorToQVector(yAxisValues[i]);
    customPlot->addGraph();
    customPlot->graph()->setData(x, y);
  }

  customPlot->xAxis->setLabel(QString::fromStdString(xAxisName));
  customPlot->yAxis->setLabel(QString::fromStdString(yAxisName));

  customPlot->rescaleAxes();
  customPlot->replot();

  this->qMainWindow_->setGeometry(100, 100, 500, 500);
  this->qMainWindow_->show();

  this->qApplication_.exec();

  customPlot->clearPlottables();
}

template<typename T>
QVector<double> PlottingUtils<T>::convertVectorToQVector(
    const std::vector<T>& stdVector) {

  QVector<double> qVector;
  for (const auto& value : stdVector) {
    qVector.push_back((double) value);
  }
  return qVector;
}

}
}
