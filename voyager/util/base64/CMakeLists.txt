set(base64_SRCS
  base64.cc
  modp_base64/modp_b64.cc
  )

add_library(voyager_util_base64 ${base64_SRCS})

install (TARGETS voyager_util_base64 DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/util/base64)