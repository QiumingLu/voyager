set(core_SRCS
  acceptor.cc
  connector.cc
  dispatch.cc
  eventloop.cc
  eventloop_thread.cc
  eventloop_threadpool.cc
  event_epoll.cc
  event_poll.cc
  event_poller.cc
  event_select.cc
  sockaddr.cc
  socket_util.cc
  tcp_client.cc
  tcp_connection.cc
  tcp_server.cc
  tcp_socket.cc
  )

add_library(mirants_core ${core_SRCS})

target_link_libraries(mirants_core mirants_port)

install(TARGETS mirants_core DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/mirants/core)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
