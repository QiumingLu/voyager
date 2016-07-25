set(core_SRCS
  acceptor.cc
  base_socket.cc
  bg_eventloop.cc
  buffer.cc
  client_socket.cc
  connector.cc
  dispatch.cc
  eventloop.cc
  event_poll.cc
  event_poller.cc
  event_select.cc
  schedule.cc
  server_socket.cc
  sockaddr.cc
  tcp_client.cc
  tcp_connection.cc
  tcp_server.cc
  timer.cc
  )

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") 
  list(APPEND core_SRCS "event_epoll.cc")
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  list(APPEND core_SRCS "event_kqueue.cc")
endif()

add_library(voyager_core ${core_SRCS})

target_link_libraries(voyager_core voyager_port)

install(TARGETS voyager_core DESTINATION lib)

set(HEADERS 
  bg_eventloop.h
  buffer.h
  callback.h
  dispatch.h
  eventloop.h
  sockaddr.h
  tcp_client.h
  tcp_connection.h
  tcp_server.h
  base_socket.h
  client_socket.h
  server_socket.h
  schedule.h
  )

install(FILES ${HEADERS} DESTINATION include/voyager/core)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
