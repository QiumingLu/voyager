set(core_SRCS
  acceptor.cc
  base_socket.cc
  client_socket.cc
  server_socket.cc
  buffer.cc
  connector.cc
  dispatch.cc
  eventloop.cc
  eventloop_thread.cc
  eventloop_threadpool.cc
  event_poll.cc
  event_poller.cc
  event_select.cc
  sockaddr.cc
  socket_util.cc
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
  buffer.h
  callback.h
  dispatch.h
  eventloop.h
  eventloop_thread.h
  eventloop_threadpool.h
  sockaddr.h
  tcp_client.h
  tcp_connection.h
  tcp_server.h
  base_socket.h
  client_socket.h
  server_socket.h
  )

install(FILES ${HEADERS} DESTINATION include/voyager/core)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
