set(util_SRCS
  logging.cc
  status.cc
  string_util.cc
  stringprintf.cc
  testharness.cc
  timeops.cc
  json/json.cc
  base64/base64.cc
  base64/modp_base64/modp_b64.cc
  )

add_library(voyager_util ${util_SRCS})

install(TARGETS voyager_util DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/util)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
