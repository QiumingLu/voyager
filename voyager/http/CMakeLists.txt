file(GLOB tmp *.cc)
set(Voyager_SRCS ${Voyager_SRCS} ${tmp})

set(Voyager_SRCS ${Voyager_SRCS} PARENT_SCOPE)

set(Voyager_HTTP_HEADERS
  http_client.h
  http_message.h
  http_request.h
  http_response.h
  http_request_parser.h
  http_response_parser.h
  http_server.h
  )

install(FILES ${Voyager_HTTP_HEADERS} DESTINATION include/voyager/http)

if (BUILD_TESTS) 
  add_subdirectory(tests)
endif()
