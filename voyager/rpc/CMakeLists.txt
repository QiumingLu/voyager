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

add_library(voyager_rpc ${rpc_SRCS})

target_link_libraries(voyager_rpc voyager_core ${PROTOBUF_LIBRARIES})

set_target_properties(voyager_rpc PROPERTIES COMPILE_FLAGS "-Wno-error -Wno-shorten-64-to-32")

install(TARGETS voyager_rpc DESTINATION lib)

set(HEADERS
  rpc_codec.h
  rpc_channel.h
  rpc_server.h
  rpc.proto
  ${PROTO_HDRS}
  )

install(FILES ${HEADERS} DESTINATION include/voyager/rpc)
