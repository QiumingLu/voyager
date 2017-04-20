add_executable(tcp_client_test tcp_client_test.cc)
target_link_libraries(tcp_client_test voyager)

add_executable(tcp_server_test tcp_server_test.cc)
target_link_libraries(tcp_server_test voyager)

add_executable(threadsafe_test threadsafe_test.cc)
target_link_libraries(threadsafe_test voyager)

add_executable(echo_server_test echo_server_test.cc)
target_link_libraries(echo_server_test voyager)

add_executable(timer_test timer_test.cc)
target_link_libraries(timer_test voyager)
