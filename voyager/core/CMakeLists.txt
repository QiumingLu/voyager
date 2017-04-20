file(GLOB tmp *.cc)
set(Voyager_SRCS ${Voyager_SRCS} ${tmp})

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(tmp 
    "${CMAKE_CURRENT_SOURCE_DIR}/event_kqueue.cc")
else()
  set(tmp 
    "${CMAKE_CURRENT_SOURCE_DIR}/event_epoll.cc" 
    "${CMAKE_CURRENT_SOURCE_DIR}/newtimer.cc")
endif()
exclude(Voyager_SRCS "${Voyager_SRCS}" ${tmp})

set(Voyager_SRCS ${Voyager_SRCS} PARENT_SCOPE)

set(Voygaer_CORE_HEADERS 
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
  list(APPEND Voygaer_CORE_HEADERS "event_epoll.h" "newtimer.h")
else()
  list(APPEND Voygaer_CORE_HEADERS "event_kqueue.h")
endif()

install(FILES ${Voygaer_CORE_HEADERS} DESTINATION include/voyager/core)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
