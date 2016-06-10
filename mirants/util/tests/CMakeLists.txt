add_executable(logging_test logging_test.cc)
add_executable(util_test    util_test.cc)
add_executable(any_test     any_test.cc)

target_link_libraries(logging_test mirants_util)
target_link_libraries(util_test    mirants_util)
target_link_libraries(any_test     mirants_util)
