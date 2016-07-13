set(port_SRCS
  mutex.cc
  thread.cc
  threadpool.cc
  )
add_library(mirants_port ${port_SRCS})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_link_libraries(mirants_port pthread)
else()
  target_link_libraries(mirants_port pthread rt)
endif()
target_link_libraries(mirants_port mirants_util)

install(TARGETS mirants_port DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/mirants/port)

if (NOT CMAKE_BUILD_NO_EXAMPLES) 
  add_subdirectory(tests)
endif()
