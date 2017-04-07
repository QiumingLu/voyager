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

if(BUILD_SHARED_LIBS)
  add_library(voyager_util SHARED ${util_SRCS})
  add_library(voyager_util_static STATIC ${util_SRCS})    
  set_target_properties(voyager_util_static PROPERTIES OUTPUT_NAME "voyager_util")    
  set_target_properties(voyager_util PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})    
  set_target_properties(voyager_util_static PROPERTIES CLEAN_DIRECT_OUTPUT ${VOYAGER_VERSION_MAJOR})   
  set_target_properties(voyager_util PROPERTIES VERSION ${VOYAGER_VERSION_STRING} SOVERSION ${VOYAGER_VERSION_MAJOR})    
        
  install(TARGETS voyager_util DESTINATION lib)
  install(TARGETS voyager_util_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else()
  add_library(voyager_util ${util_SRCS})
  install(TARGETS voyager_util DESTINATION lib)
endif()

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/voyager/util)

set(
  json_Headers
  json/json.h
  json/json_value.h
  )
install(FILES ${json_Headers} DESTINATION include/voyager/util/json)
install(FILES base64/base64.h DESTINATION include/voyager/util/base64)

if (BUILD_TESTS)
  add_subdirectory(tests)
endif()
