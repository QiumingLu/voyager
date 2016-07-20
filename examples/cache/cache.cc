#include "examples/cache/cache.h"

namespace voyager {
namespace cache {

struct LRUHandle {
  void* value;
  void (*deleter)(const Slice& key, void void* value);
  LRUHandle* nex_hash;
  LRUHandle* next;
  LRUHandle* prev;
  size_t charge;
  size_t key_length;
  bool in_cache;
  uint32_t refs;
  uint32_t hash;
  char key_data[1];
};
