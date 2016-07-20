#include "examples/cache/cache.h"

namespace voyager {
namespace cache {

struct LRUHandle {
  void* value;
  void (*deleter)(const Slice& key, void* value);
  LRUHandle* nex_hash;
  LRUHandle* next;
  LRUHandle* prev;
  size_t charge;
  size_t key_length;
  bool in_cache;       // Whether entry is in the cache.
  uint32_t refs;       // References, including cache reference, if present.
  uint32_t hash;       // Hash of key(); used for fast sharding and comparisons
  char key_data[1];    // Beginning of key

  Slice key() const {
    // For cheaper lookups, we allow a temporary Handle object
    // to store a pointer to a key in "value".
    if (next == this) {
      return *(reinterpret_cast<Slice*>(value));
    } else {
      return Slice(key_data, key_length);
    }
  }
};

class HandleTable {
 public:
   HandleTable() { }

 private:
};

}  // namespace cache
}  // namespace voyager
