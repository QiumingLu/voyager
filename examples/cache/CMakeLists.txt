set (cache_SRCS
  arena.cc
  lru_cache.cc
  memtable.cc
  )

add_library(examples_cache ${cache_SRCS})
target_link_libraries(examples_cache voyager_core)

set(HEADERS
   arena.h
   lru_cache.h
   memtable.h
   skiplist.h
   random.h)
