set(port_SRCS
  mutex.cc
  thread.cc
  )

add_library(voyager_port SHARED ${port_SRCS})
add_library(voyager_port_static STATIC ${port_SRCS})
set_target_properties(voyager_port_static PROPERTIES OUTPUT_NAME "voyager_port")
set_target_properties(voyager_port PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(voyager_port_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(voyager_port PROPERTIES VERSION 1.0 SOVERSION 1)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(voyager_port pthread rt)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_link_libraries(voyager_port pthread)
endif()

target_link_libraries(voyager_port voyager_util)

install(TARGETS voyager_port_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/port)

if (NOT CMAKE_BUILD_NO_EXAMPLES) 
  add_subdirectory(tests)
endif()
