aux_source_directory(. core_SRCS)

add_library(mirants_core ${core_SRCS})

add_subdirectory(tests)
