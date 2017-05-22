cmake_minimum_required(VERSION 3.1)

add_executable(compare_bps_zig_zag compare_bps_zig_zag.cc ${QCustomPlot_SOURCE_FILES}
                                                           ${PlottingUtils_SOURCE_FILES})
target_link_libraries(compare_bps_zig_zag ${BPS_LINK_LIBRARIES})
