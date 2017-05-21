add_executable(utils_tests utils_tests.cc)
target_link_libraries(utils_tests gtest gmock)

add_executable(pdmp_builder_tests pdmp_builder_tests.cc)
target_link_libraries(pdmp_builder_tests gtest gmock)

add_test(NAME utils_tests COMMAND utils_tests)
add_test(NAME pdmp_builder_tests COMMAND pdmp_builder_tests)

add_subdirectory(distributions)
add_subdirectory(bps)
