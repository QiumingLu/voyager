include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS rpc.proto)

set(rpc_SRCS
  rpc_codec.cc
  rpc_channel.cc
  rpc_server.cc
  ${PROTO_SRCS}
  )

if(CMAKE_BUILD_SHARED_LIBS)
  add_library(voyager_rpc SHARED ${rpc_SRCS})
  add_library(voyager_rpc_static STATIC ${rpc_SRCS})    
  set_target_properties(voyager_rpc_static PROPERTIES OUTPUT_NAME "voyager_rpc")    
  set_target_properties(voyager_rpc PROPERTIES CLEAN_DIRECT_OUTPUT 1)   
  set_target_properties(voyager_rpc_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)    
  set_target_properties(voyager_rpc PROPERTIES VERSION 1.6 SOVERSION 1)

  target_link_libraries(voyager_rpc voyager_core ${PROTOBUF_LIBRARIES})

  #  set_target_properties(voyager_rpc PROPERTIES COMPILE_FLAGS "-Wno-error -Wno-shorten-64-to-32")

  install(TARGETS voyager_rpc DESTINATION lib)
  install(TARGETS voyager_rpc_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else()
  add_library(voyager_rpc ${rpc_SRCS})
  target_link_libraries(voyager_rpc voyager_core)
  install(TARGETS voyager_rpc DESTINATION lib)
endif()

set(HEADERS
  rpc_codec.h
  rpc_channel.h
  rpc_server.h
  rpc.proto
  ${PROTO_HDRS}
  )

install(FILES ${HEADERS} DESTINATION include/voyager/rpc)
