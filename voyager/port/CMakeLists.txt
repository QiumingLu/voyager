set(port_SRCS
  mutex.cc
  thread.cc
  )

add_library(voyager_port ${port_SRCS})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(voyager_port pthread rt)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_link_libraries(voyager_port pthread)
endif()

target_link_libraries(voyager_port voyager_util)

install(TARGETS voyager_port DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/port)

if (NOT CMAKE_BUILD_NO_EXAMPLES) 
  add_subdirectory(tests)
endif()
