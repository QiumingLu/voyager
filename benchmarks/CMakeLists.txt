add_executable(bench bench.cc)
target_link_libraries(bench mirants_core)

add_executable(client client.cc)
target_link_libraries(client mirants_core)

add_executable(server server.cc)
target_link_libraries(server mirants_core)
