set(port_SRCS
  mutex.cc
  thread.cc
  threadpool.cc
  )

add_library(mirants_port ${port_SRCS})

install(TARGETS mirants_port DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/mirants/port)

target_link_libraries(mirants_port pthread rt)
target_link_libraries(mirants_port mirants_util)

if (NOT CMAKE_BUILD_NO_EXAMPLES) 
  add_subdirectory(tests)
endif()
