#include "box_plot.h"

#include <QMainWindow>
#include <qcustomplot.h>

#include "gsl_wrappers/statistics_wrappers.h"

#include "utils.h"

namespace {

// We consider datapoints outliers if one of the following inequalities hold:
// datapoint >= thirdQuartile * 1.5 or datapoint <= firstQuartile / 1.5
QVector<double> getOutliers(
  const std::vector<double>& data,
  const double& firstQuartile,
  const double& thirdQuartile) {

  QVector<double> outliers;
  for (const double& datapoint : data) {
    if (datapoint >= thirdQuartile * 1.5 || datapoint <= firstQuartile / 1.5) {
      outliers.push_back(datapoint);
    }
  }
  return outliers;
}

}

namespace pdmp {
namespace analysis {

void plotBoxPlot(
    const std::vector<std::vector<double>>& data,
    const std::vector<std::string>& names,
    const std::string& fileName) {

  QCustomPlot* customPlot = new QCustomPlot();
  QMainWindow qMainWindow;
  qMainWindow.setCentralWidget(customPlot);

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
    bps::GslStatisticsWrappers<double>::calculateStatisticsForBoxPlots(
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

  handleImageOutput(*customPlot, qMainWindow, fileName);
}

}
}
