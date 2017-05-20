#include "utils.h"

#include <QDir>

#include "qapplication.h"

namespace pdmp {
namespace analysis {

// Creates required directories if they do not exist, along
// all the given path.
void createRequiredDirectories(const QString& filePath) {
  QFileInfo fileInfo(filePath);
  QDir fileDir = fileInfo.dir();
  QDir currentDir;
  currentDir.mkpath(fileDir.absolutePath());
}

// Shows the image on screen.
void showImageOnScreen(QMainWindow& qMainWindow) {
  qMainWindow.setGeometry(100, 100, 500, 500);
  qMainWindow.show();
  qApplication.exec();
}

// Shows the image on screen, if no file name is provided.
// Otherwise, saves the image.
void handleImageOutput(
  QCustomPlot& qCustomPlot,
  QMainWindow& qMainWindow,
  const std::string& fileName) {

  if (fileName == "") {
    showImageOnScreen(qMainWindow);
  } else {
    QString filePath = QString::fromStdString(fileName + ".png");
    createRequiredDirectories(filePath);
    qCustomPlot.savePng(filePath, 800, 800);
  }
}

}
}
