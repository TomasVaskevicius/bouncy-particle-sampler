add_executable(utils_tests utils_tests.cc)
target_link_libraries(utils_tests gtest gmock)

add_test(NAME utils_tests COMMAND utils_tests)

