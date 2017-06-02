cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(gaussian_chain gaussian_chain.cc)
target_link_libraries(gaussian_chain ${BPS_LINK_LIBRARIES})
