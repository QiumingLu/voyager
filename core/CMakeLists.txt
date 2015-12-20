set(core_SRCS
  acceptor.cc
  connector.cc
  event_defaultpoller.cc
  event_epoll.cc
  event_poll.cc
  event_poller.cc
  event_select.cc
  eventloop.cc
  sockaddr.cc
  socket_util.cc
  tcp_client.cc
  tcp_server.cc
  tcp_socket.cc
  )

add_library(mirants_core ${core_SRCS})

add_subdirectory(tests)
