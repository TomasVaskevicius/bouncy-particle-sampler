cmake_minimum_required(VERSION 3.1)

add_executable(benchmarking_example benchmarking_example.cc
                                    ${QCustomPlot_SOURCE_FILES})
target_link_libraries(benchmarking_example ${BPS_LINK_LIBRARIES})
