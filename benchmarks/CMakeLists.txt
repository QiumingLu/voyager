add_executable(bench bench.cc)
target_link_libraries(bench voyager)

add_executable(client client.cc)
target_link_libraries(client voyager)

add_executable(server server.cc)
target_link_libraries(server voyager)
