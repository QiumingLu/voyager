aux_source_directory(. port_SRCS)
add_library(mirants_port ${port_SRCS})

target_link_libraries(mirants_port pthread rt)

add_subdirectory(tests)
