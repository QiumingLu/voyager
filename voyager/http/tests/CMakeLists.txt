add_executable(httpserver httptest.cc)
add_executable(httpclient httpclient_test.cc)

target_link_libraries(httpserver voyager)
target_link_libraries(httpclient voyager)
