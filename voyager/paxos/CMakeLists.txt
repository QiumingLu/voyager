set(INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB PROTP_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
set(PROTOC ${PROJECT_SOURCE_DIR}/third_party/protobuf/bin/protoc)
set(PROTOC_C_OUT_FLAG --cpp_out)
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "PROTOC = ${PROTOC}")

foreach(PROTO_FILE ${PROTP_INPUT})
  get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
  set(CUR_PROTO_GEN
    ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h
    ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc
    )
  set(PROTO_GEN
    ${PROTO_GEN}
    ${CUR_PROTO_GEN}
    )
  message(STATUS "CUR_PROTO_GEN = ${CUR_PROTO_GEN}")

  add_custom_command(
    OUTPUT ${CUR_PROTO_GEN}
    COMMAND ${PROTOC} ${PROTO_FILE} ${PROTOC_C_OUT_FLAG} ${PROTO_GEN_DIR} -I${INPUT_DIR}
    DEPENDS ${PROTOC} ${PROTO_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
    )
endforeach(PROTO_FILE ${PROTP_INPUT})

file(GLOB PROTO_GEN_H ${PROTO_GEN_DIR}/*.pb.h)
file(GLOB PROTO_GEN_CC ${PROTO_GEN_DIR}/*.pb.cc)

set_source_files_properties(${PROTO_GEN} COMPILE_FLAGS "-Wno-conversion")
include_directories(${PROJECT_BINARY_DIR})

set (paxos_SRCS
  acceptor.cc
  config.cc
  counter.cc
  group.cc
  instance.cc
  learner.cc
  node.cc
  nodeinfo.cc
  #paxos_message.cc
  proposer.cc
  runloop.cc
  transfer.cc
  transfer_station.cc
  network/messager.cc
  network/network.cc
  storage/db.cc
  storage/multi_db.cc
  ${PROTO_GEN_CC}
  )

add_library(voyager_paxos ${paxos_SRCS})

target_link_libraries(voyager_paxos voyager_http)

install(TARGETS voyager_paxos DESTINATION lib)

set(HEADERS
  options.h
  nodeinfo.h
  )

install(FILES ${HEADERS} DESTINATION include/voyager/paxos)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
