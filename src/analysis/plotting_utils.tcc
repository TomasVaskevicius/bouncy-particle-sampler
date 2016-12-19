#pragma once

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

  QCPCurve samplePath(customPlot->xAxis, customPlot->yAxis);
  samplePath.setData(x, y);

  customPlot->addGraph();
  customPlot->xAxis->rescale();
  customPlot->yAxis->rescale();
  customPlot->yAxis->setScaleRatio(customPlot->xAxis, 1.0);
  customPlot->replot();

  this->qMainWindow_->setGeometry(100, 100, 500, 500);
  this->qMainWindow_->show();

  this->qApplication_.exec();
}

}
}
