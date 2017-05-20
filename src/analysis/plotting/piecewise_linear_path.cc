#include "piecewise_linear_path.h"

#include <QMainWindow>
#include <qcustomplot.h>

#include "utils.h"

namespace pdmp {
namespace analysis {

void plotTwoDimensionalPiecewiseLinearSamplePath(
  const std::vector<double>& x,
  const std::vector<double>& y,
  const std::string& fileName) {

  QCustomPlot* qCustomPlot = new QCustomPlot();
  QMainWindow qMainWindow;
  qMainWindow.setCentralWidget(qCustomPlot);

  QCPCurve* samplePath = new QCPCurve(qCustomPlot->xAxis, qCustomPlot->yAxis);
  samplePath->setData(
    QVector<double>::fromStdVector(x),
    QVector<double>::fromStdVector(y));

  qCustomPlot->addGraph();
  qCustomPlot->xAxis->rescale();
  qCustomPlot->yAxis->rescale();
  qCustomPlot->yAxis->setScaleRatio(qCustomPlot->xAxis, 1.0);
  qCustomPlot->replot();

  handleImageOutput(*qCustomPlot, qMainWindow, fileName);
}

}
}
