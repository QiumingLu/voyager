#ifndef EXAMPLES_CACHE_ARENA_H_
#define EXAMPLES_CACHE_ARENA_H_

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include <atomic>
#include <vector>

namespace voyager {
namespace cache {

class Arena {
 public:
  Arena();
  ~Arena();

  char* Allocate(size_t bytes);
  char* AllocateAligned(size_t bytes);
  size_t MemoryUsage() const {
    return memory_usage_.load(std::memory_order_relaxed);
  }

 private:
  char* AllocateFallback(size_t bytes);
  char* AllocateNewBlock(size_t block_bytes);

  char* alloc_ptr_;
  size_t alloc_bytes_remaining_;

  std::vector<char*> blocks_;

  std::atomic<size_t> memory_usage_;

  // No copying allow
  Arena(const Arena&);
  void operator=(const Arena&);
};

inline char* Arena::Allocate(size_t bytes) {
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }

  return AllocateFallback(bytes);
}

}  // namespace cache
}  // namespace voyager

#endif  // EXAMPLES_CACHE_ARENA_H_
