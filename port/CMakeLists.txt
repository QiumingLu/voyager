set(port_SRCS
  mutex.cc
  thread.cc
  threadpool.cc
  )

add_library(mirants_port ${port_SRCS})

target_link_libraries(mirants_port pthread rt)

add_subdirectory(tests)
