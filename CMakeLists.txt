cmake_minimum_required(VERSION 3.1)

project(BouncyParticleSampler)

# Enable C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

# Find the GNU Scientific Library.
find_package(GSL REQUIRED)

# Find the Qt5 libraries (required by QCustomPlot) and turn on automoc.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

# For using std::thread we will need to link with -pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Fin Eigen, required for our linear algebra support.
find_package(Eigen3 REQUIRED)

# Find CVODES and Boost (required by the Stan-Math library).
find_package(CVODES REQUIRED)
find_package(Boost REQUIRED)

# Add all project's include directories.
set(QCustomPlot_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/qcustomplot_2.0.0_beta)
set(StanMath_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/stan_math)
include_directories(${PROJECT_SOURCE_DIR}/src)
# We add third party headers as system headers to supress g++ warnings.
include_directories(system ${StanMath_INCLUDE_DIR})
include_directories(system ${GSL_INCLUDE_DIRS})
include_directories(system ${Boost_INCLUDE_DIR})
include_directories(system ${CVODES_INCLUDE_DIR})
include_directories(system ${QCustomPlot_INCLUDE_DIR})
include_directories(system ${EIGEN3_INCLUDE_DIR})

# Add QCustomPlot source file.
set(QCustomPlot_SOURCE_FILES ${QCustomPlot_INCLUDE_DIR}/qcustomplot.cpp)

# Add plotting utils source files.
set(PlottingUtils_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/analysis/plotting/piecewise_linear_path.cc
                               ${PROJECT_SOURCE_DIR}/src/analysis/plotting/utils.cc)

# Create a variable for holding all necessary link libraries.
set(BPS_LINK_LIBRARIES ${CVODES_LIBRARIES}
                       ${GSL_LIBRARIES}
                       Qt5::Widgets
                       Qt5::PrintSupport
                       Qt5::Gui
                       Qt5::Core
                       Threads::Threads)

add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)
