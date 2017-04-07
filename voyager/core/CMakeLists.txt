set(core_SRCS
  base_socket.cc
  bg_eventloop.cc
  buffer.cc
  client_socket.cc
  dispatch.cc
  eventloop.cc
  event_poll.cc
  event_poller.cc
  event_select.cc
  schedule.cc
  server_socket.cc
  sockaddr.cc
  tcp_acceptor.cc
  tcp_connector.cc
  tcp_client.cc
  tcp_connection.cc
  tcp_server.cc
  timerlist.cc
  )

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") 
  list(APPEND core_SRCS "event_epoll.cc" "newtimer.cc")
else()
  list(APPEND core_SRCS "event_kqueue.cc")
endif()

if (BUILD_SHARED_LIBS)
  add_library(voyager_core SHARED ${core_SRCS})
  add_library(voyager_core_static STATIC ${core_SRCS})    
  set_target_properties(voyager_core_static PROPERTIES OUTPUT_NAME "voyager_core")    
  set_target_properties(voyager_core PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})    
  set_target_properties(voyager_core_static PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})   
  set_target_properties(voyager_core PROPERTIES VERSION ${VOYAGER_VERSION_STRING} SOVERSION ${VOYAGER_VERSION_MAJOR})

  target_link_libraries(voyager_core voyager_port)

  install(TARGETS voyager_core DESTINATION lib)
  install(TARGETS voyager_core_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else()
  add_library(voyager_core ${core_SRCS})
  target_link_libraries(voyager_core voyager_port)
  install(TARGETS voyager_core DESTINATION lib)
endif()

set(HEADERS 
  base_socket.h
  bg_eventloop.h
  buffer.h
  callback.h
  client_socket.h
  dispatch.h
  eventloop.h
  event_poll.h
  event_select.h
  schedule.h
  server_socket.h
  sockaddr.h
  tcp_client.h
  tcp_connection.h
  tcp_server.h
  timerlist.h
  )

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") 
  list(APPEND HEADERS "event_epoll.h" "newtimer.h")
else()
  list(APPEND core_SRCS "event_kqueue.h")
endif()

install(FILES ${HEADERS} DESTINATION include/voyager/core)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
