set (paxos_SRCS
  acceptor.cc
  base.cc
  counter.cc
  group.cc
  instance.cc
  learner.cc
  node.cc
  paxos_message.cc
  proposer.cc
  state_machine.cc
  )

add_library(voyager_paxos ${paxos_SRCS})

target_link_libraries(voyager_paxos voyager_http)

install(TARGETS voyager_paxos DESTINATION lib)

file(GLOB HEADERS ".h")
install(FILES ${HEADERS} DESTINATION include/voyager/paxos)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
