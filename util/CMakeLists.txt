set(util_SRCS
  logging.cc
  status.cc
  string_util.cc
  stringprintf.cc
  testharness.cc
  timestamp.cc
  )

add_library(mirants_util ${util_SRCS})

install(TARGETS mirants_util DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/mirants/util)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
