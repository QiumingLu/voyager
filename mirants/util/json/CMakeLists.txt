set(json_SRCS
  json.cc
  )

add_library(mirants_util_json ${json_SRCS})

install (TARGETS mirants_util_json DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/mirants/util/json)