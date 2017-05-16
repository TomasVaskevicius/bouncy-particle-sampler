add_executable(batch_means_tests batch_means_tests.cc)
target_link_libraries(batch_means_tests gtest gmock ${GSL_LIBRARIES})

add_executable(mean_estimators_tests mean_estimators_tests.cc)
target_link_libraries(mean_estimators_tests gtest gmock ${GSL_LIBRARIES})

add_test(NAME batch_means_tests COMMAND batch_means_tests)
add_test(NAME mean_estimators_tests COMMAND mean_estimators_tests)
