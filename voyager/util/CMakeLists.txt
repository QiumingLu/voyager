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

add_library(voyager_util SHARED ${util_SRCS})
add_library(voyager_util_static STATIC ${util_SRCS})    
set_target_properties(voyager_util_static PROPERTIES OUTPUT_NAME "voyager_util")    
set_target_properties(voyager_util PROPERTIES CLEAN_DIRECT_OUTPUT 1)    
set_target_properties(voyager_util_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)   
set_target_properties(voyager_util PROPERTIES VERSION 1.0 SOVERSION 1)    
        
install(TARGETS voyager_util DESTINATION lib)
install(TARGETS voyager_util_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/util)

set(
  json_Headers
  json/json.h
  json/json_value.h
  base64/base64.h
  )
install(FILES ${json_Headers} DESTINATION include/voyager/util)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
