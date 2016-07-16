add_executable(bench bench.cc)
target_link_libraries(bench voyager_core)

add_executable(client client.cc)
target_link_libraries(client voyager_core)

add_executable(server server.cc)
target_link_libraries(server voyager_core)
