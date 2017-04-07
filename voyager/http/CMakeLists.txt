set(http_SRCS
  http_client.cc
  http_message.cc
  http_request.cc
  http_response.cc
  http_request_parser.cc
  http_response_parser.cc
  http_server.cc
  )

if(BUILD_SHARED_LIBS)
  add_library(voyager_http SHARED ${http_SRCS}) 
  add_library(voyager_http_static STATIC ${http_SRCS})    
  set_target_properties(voyager_http_static PROPERTIES OUTPUT_NAME "voyager_http")    
  set_target_properties(voyager_http PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})    
  set_target_properties(voyager_http_static PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})   
  set_target_properties(voyager_http PROPERTIES VERSION ${VOYAGER_VERSION_STRING} SOVERSION ${VOYAGER_VERSION_MAJOR})

  target_link_libraries(voyager_http voyager_core)

  install(TARGETS voyager_http DESTINATION lib)
  install(TARGETS voyager_http_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else()
  add_library(voyager_http ${http_SRCS})
  target_link_libraries(voyager_http voyager_core)
  install(TARGETS voyager_http DESTINATION lib)
endif()

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

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
