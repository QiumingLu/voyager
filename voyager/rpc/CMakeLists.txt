set(PROTOC ${CMAKE_PROTOBUF_PATH}/bin/protoc)    
set(PROTOC_CPP_OUT --cpp_out)    
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})    
    
file(GLOB PROTP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)    
    
foreach(PROTO_FILE ${PROTP_FILES})    
  get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)    
  set(CUR_PROTO_GEN   
    ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h   
    ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc    
    )   
  set(PROTO_GEN   
    ${PROTO_GEN}    
    ${CUR_PROTO_GEN}    
    )   
    
  add_custom_command(   
    OUTPUT ${CUR_PROTO_GEN}   
    COMMAND ${PROTOC}     
    ARGS ${PROTOC_CPP_OUT} ${PROTO_GEN_DIR} ${PROTO_FILE} -I${CMAKE_CURRENT_SOURCE_DIR}    
    DEPENDS ${PROTO_FILE}   
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}   
    )   
endforeach()    

include_directories(${PROJECT_BINARY_DIR})    
   
set(rpc_SRCS
  rpc_codec.cc
  rpc_channel.cc
  rpc_server.cc
  ${PROJECT_BINARY_DIR}/voyager/rpc/rpc.pb.cc
  )

add_library(voyager_rpc ${rpc_SRCS})
set_target_properties(voyager_rpc PROPERTIES COMPILE_FLAGS "-Wno-error -Wno-shorten-64-to-32 -Wno-deprecated-declarations")
target_link_libraries(voyager_rpc voyager_core protobuf)

install(TARGETS voyager_rpc DESTINATION lib)

set(HEADERS
  rpc_codec.h
  rpc_channel.h
  rpc_server.h
  rpc.proto
  ${PROJECT_BINARY_DIR}/voyager/rpc/rpc.pb.h
  )

install(FILES ${HEADERS} DESTINATION include/voyager/rpc)
