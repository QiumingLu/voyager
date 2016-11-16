set (paxos_SRCS
  acceptor.cc
  config.cc
  counter.cc
  group.cc
  instance.cc
  learner.cc
  log_storage.cc
  messager.cc
  network.cc
  node.cc
  nodeinfo.cc
  paxos_message.cc
  proposer.cc
  #  state_machine.cc
  runloop.cc
  transfer.cc
  transfer_station.cc
  storage/db.cc
  )

add_library(voyager_paxos ${paxos_SRCS})

target_link_libraries(voyager_paxos voyager_http)

include_directories(/Users/mirantslu/Development/voyager/third_party/leveldb/include)
link_libraries(/Users/mirantslu/Development/voyager/third_party/leveldb/out-static/libleveldb.a)

install(TARGETS voyager_paxos DESTINATION lib)

set(HEADERS
  options.h
  nodeinfo.h
  )
install(FILES ${HEADERS} DESTINATION include/voyager/paxos)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
