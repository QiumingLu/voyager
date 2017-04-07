set(port_SRCS
  mutex.cc
  thread.cc
  )

if(BUILD_SHARED_LIBS)
  add_library(voyager_port SHARED ${port_SRCS})
  add_library(voyager_port_static STATIC ${port_SRCS})		
  set_target_properties(voyager_port_static PROPERTIES OUTPUT_NAME "voyager_port")		
  set_target_properties(voyager_port PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})		
  set_target_properties(voyager_port_static PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})		
  set_target_properties(voyager_port PROPERTIES VERSION ${VOYAGER_VERSION_STRING} SOVERSION ${VOYAGER_VERSION_MAJOR})

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

if (BUILD_TESTS) 
  add_subdirectory(tests)
endif()
