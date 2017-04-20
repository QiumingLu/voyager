file(GLOB tmp *.cc)
set(Voyager_SRCS ${Voyager_SRCS} ${tmp})

set(Voyager_SRCS ${Voyager_SRCS} PARENT_SCOPE)

file(GLOB Voyager_PORT_HEADERS *.h)
install(FILES ${Voyager_PORT_HEADERS} DESTINATION include/voyager/port)

if (BUILD_TESTS) 
  add_subdirectory(tests)
endif()
