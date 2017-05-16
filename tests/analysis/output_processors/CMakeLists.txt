add_executable(batch_means_tests batch_means_tests.cc)
target_link_libraries(batch_means_tests gtest gmock ${GSL_LIBRARIES})

add_test(NAME batch_means_tests COMMAND batch_means_tests)
