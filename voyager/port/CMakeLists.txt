set(port_SRCS
  mutex.cc
  thread.cc
  )

if(CMAKE_BUILD_SHARED_LIB)
  add_library(voyager_port SHARED ${port_SRCS})
  add_library(voyager_port_static STATIC ${port_SRCS})		
  set_target_properties(voyager_port_static PROPERTIES OUTPUT_NAME "voyager_port")		
  set_target_properties(voyager_port PROPERTIES CLEAN_DIRECT_OUTPUT 1)		
  set_target_properties(voyager_port_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)		
  set_target_properties(voyager_port PROPERTIES VERSION 1.5 SOVERSION 1)

  target_link_libraries(voyager_port voyager_util pthread)

  install(TARGETS voyager_port DESTINATION lib)
  install(TARGETS voyager_port_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else()
  add_library(voyager_port ${port_SRCS})
  target_link_libraries(voyager_port voyager_util pthread)
  install(TARGETS voyager_port DESTINATION lib)
endif()


file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/port)

if (NOT CMAKE_BUILD_NO_EXAMPLES) 
  add_subdirectory(tests)
endif()
