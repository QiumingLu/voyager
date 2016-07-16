set(json_SRCS
  json.cc
  )

add_library(voyager_util_json ${json_SRCS})

install (TARGETS voyager_util_json DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/util/json)