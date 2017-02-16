cmake_minimum_required(VERSION 3.1)

project(BouncyParticleSampler)

# Enable C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the GNU Scientific Library.
find_package(GSL REQUIRED)

# Find the Qt5 libraries (required by QCustomPlot) and turn on automoc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

# For using std::thread we will need to link with -pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add all project's include directories.
set(Eigen_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/eigen_3.3.0)
set(QCustomPlot_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/qcustomplot_2.0.0_beta)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${GSL_INCLUDE_DIRS})
# We add third party headers as system headers to supress g++ warnings.
include_directories(system ${Eigen_INCLUDE_DIR})
include_directories(system ${QCustomPlot_INCLUDE_DIR})

# Add QCustomPlot source file.
set(QCustomPlot_SOURCE_FILES ${QCustomPlot_INCLUDE_DIR}/qcustomplot.cpp)

# Create a variable for holding all necessary link libraries.
set(BPS_LINK_LIBRARIES ${GSL_LIBRARIES}
                       Qt5::Widgets
                       Qt5::PrintSupport
                       Qt5::Gui
                       Qt5::Core
                       Threads::Threads)

add_subdirectory(examples)
