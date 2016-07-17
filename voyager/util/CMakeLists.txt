set(util_SRCS
  logging.cc
  status.cc
  string_util.cc
  stringprintf.cc
  testharness.cc
  timestamp.cc
  json/json.cc
  base64/base64.cc
  base64/modp_base64/modp_b64.cc
  )

add_library(voyager_util ${util_SRCS})

install(TARGETS voyager_util DESTINATION lib)

set(HEADERS
  any.h
  logging.h
  slice.h
  status.h
  base64/base64.h
  json/json.h
  json/json_value.h
  )

install(FILES ${HEADERS} DESTINATION include/voyager)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
