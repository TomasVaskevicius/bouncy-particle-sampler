add_executable(pdmp_runner_tests pdmp_runner_tests.cc)
target_link_libraries(pdmp_runner_tests gtest gmock)

add_executable(timed_runner_tests timed_runner_tests.cc)
target_link_libraries(timed_runner_tests gtest gmock)

add_test(NAME pdmp_runner_tests COMMAND pdmp_runner_tests)
add_test(NAME timed_runner_tests COMMAND timed_runner_tests)

add_subdirectory(output_processors)
