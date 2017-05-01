add_executable(flow_tests flow_tests.cc)
target_link_libraries(flow_tests gtest gmock)

add_executable(pdmp_tests pdmp_tests.cc)
target_link_libraries(pdmp_tests gtest gmock)

add_test(NAME flow_tests COMMAND flow_tests)
add_test(NAME pdmp_tests COMMAND pdmp_tests)
