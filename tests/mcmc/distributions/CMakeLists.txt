add_executable(gaussian_tests gaussian_tests.cc)
target_link_libraries(gaussian_tests gtest gmock)

add_test(NAME gaussian_tests COMMAND gaussian_tests)

