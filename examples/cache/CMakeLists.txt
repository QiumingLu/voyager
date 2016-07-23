set (cache_SRCS
  arena.cc
  cache.cc
  hash.cc
  )

add_library(examples_cache ${cache_SRCS})
target_link_libraries(examples_cache voyager_core)

set(HEADERS
   arena.h
   cache.h
   coding.h)
