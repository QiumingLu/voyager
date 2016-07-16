set(util_SRCS
  logging.cc
  status.cc
  string_util.cc
  stringprintf.cc
  testharness.cc
  timestamp.cc
  )

add_library(voyager_util ${util_SRCS})

install(TARGETS voyager_util DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/util)

add_subdirectory(json)
add_subdirectory(base64)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
