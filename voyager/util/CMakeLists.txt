file(GLOB tmp *.cc)
set(base64 
  ${CMAKE_CURRENT_SOURCE_DIR}/base64/base64.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/base64/modp_base64/modp_b64.cc
  )
set(json 
  ${CMAKE_CURRENT_SOURCE_DIR}/json/json.cc
  )

set(Voyager_SRCS 
  ${Voyager_SRCS} 
  ${tmp}
  ${base64}
  ${modp_base64}
  )

set(Voyager_SRCS ${Voyager_SRCS} PARENT_SCOPE)

file(GLOB Voyager_UTIL_HEADERS *.h)
install(FILES ${Voyager_UTIL_HEADERS} DESTINATION include/voyager/util)

set(
  Voyager_BASE64_HEADERS
  base64/base64.h
  )
install(FILES ${Voyager_BASE64_HEADERS} DESTINATION include/voyager/util/base64)

set(
  Voyager_JSON_HEADERS
  json/json.h
  json/json_value.h
  )
install(FILES ${Voyager_JSON_HEADERS} DESTINATION include/voyager/util/json)


if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
