set (paxos_SRCS
  acceptor.cc
  counter.cc
  learner.cc
  proposer.cc
  )

add_library(voyager_paxos ${paxos_SRCS})

target_link_libraries(voyager_core)

install(TARGETS voyager_paxos DESTINATION lib)

file(GLOB HEADERS ".h")
install(FILES ${HEADERS} DESTINATION include/voyager/paxos)

if (NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()
