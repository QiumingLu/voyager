set(http_SRCS
  request.cc
  response.cc
  httpserver.cc
  )

add_library(examples_http ${http_SRCS})
target_link_libraries(examples_http mirants_core)

set(HEADERS
  request.h
  response.h
  httpserver.h
  )

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
