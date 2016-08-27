add_executable(httpserver httptest.cc)
add_executable(httpclient httpclient_test.cc)

target_link_libraries(httpserver voyager_http)
target_link_libraries(httpclient voyager_http)
