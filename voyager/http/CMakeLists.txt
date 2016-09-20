set(http_SRCS
  http_client.cc
  http_message.cc
  http_request.cc
  http_response.cc
  http_request_parser.cc
  http_response_parser.cc
  http_server.cc
  )

add_library(voyager_http ${http_SRCS})

target_link_libraries(voyager_http voyager_core)

install(TARGETS voyager_http DESTINATION lib)

set(HEADERS
  http_client.h
  http_message.h
  http_request.h
  http_response.h
  http_request_parser.h
  http_response_parser.h
  http_server.h
  )

install(FILES ${HEADERS} DESTINATION include/voyager/http)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
