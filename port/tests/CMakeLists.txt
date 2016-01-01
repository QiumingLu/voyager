add_executable(atomic_test atomic_test.cc)
add_executable(thread_test thread_test.cc)
add_executable(singleton_test singleton_test.cc)
add_test(NAME singleton_test COMMAND singleton_test)

target_link_libraries(atomic_test mirants_port)
target_link_libraries(thread_test mirants_port)
target_link_libraries(singleton_test mirants_port)
