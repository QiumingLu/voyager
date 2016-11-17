set (paxos_SRCS
  acceptor.cc
  config.cc
  counter.cc
  group.cc
  instance.cc
  learner.cc
  node.cc
  nodeinfo.cc
  paxos_message.cc
  proposer.cc
  runloop.cc
  transfer.cc
  transfer_station.cc
  network/messager.cc
  network/network.cc
  storage/db.cc
  storage/multi_db.cc
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
