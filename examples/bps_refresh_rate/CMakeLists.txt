cmake_minimum_required(VERSION 3.1)

add_executable(bps_refresh_rate bps_refresh_rate.cc ${QCustomPlot_SOURCE_FILES}
                                                    ${PlottingUtils_SOURCE_FILES})
target_link_libraries(bps_refresh_rate ${BPS_LINK_LIBRARIES})
