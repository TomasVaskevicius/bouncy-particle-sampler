#pragma once

#include <QMainWindow>
#include <qcustomplot.h>

namespace pdmp {
namespace analysis {

void createRequiredDirectories(const QString& filePath);

void showImageOnScreen(QMainWindow& qMainWindow);

void handleImageOutput(
  QCustomPlot& qCustomPlot,
  QMainWindow& qMainWindow,
  const std::string& fileName);

}
}
