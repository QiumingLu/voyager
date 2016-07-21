#include "examples/cache/cache.h"

namespace voyager {
namespace cache {

struct LRUHandle {
  void* value;
  void (*deleter)(const Slice& key, void* value);
  LRUHandle* next_hash;
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
   HandleTable() : length_(0), elems_(0), list_(NULL) { Resize(); }
   ~HandleTable() { delete[] list_; }

   LRUHandle* Lookup(const Slice& key, uint32_t hash) {
     return *FindPointer(key, hash);
   }

   LRUHandle* Insert(LRUHandle* h) {
     LRUHandle** ptr = FindPointer(h->key(), h->hash);
     LRUHandle* old = *ptr;
     h->next_hash = (old == NULL ? NULL : old->next_hash);
     *ptr = h;
     if (old == NULL) {
       ++elems_;
       if (elems_ > length_) {
         Resize();
       }
     }
     return old;
   }

   LRUHandle* Remove(const Slice& key, uint32_t hash) {
     LRUHandle** ptr = FindPointer(key, hash);
     LRUHandle* result = *ptr;
     if (result != NULL) {
       *ptr = result->next_hash;
       --elems_;
     }
     return result;
   }

 private:
  uint32_t length_;
  uint32_t elems_;
  LRUHandle** list_;

  LRUHandle** FindPointer(const Slice& key, uint32_t hash) {
    LRUHandle** ptr = &list_[hash & (length_ - 1)];
    while (*ptr != NULL &&
           ((*ptr)->hash != hash || key != (*ptr)->key())) {
      ptr = &(*ptr)->next_hash;
    }
    return ptr;    
  }

  void Resize() {
    uint32_t new_length = 4;
    while (new_length < elems_) {
      new_length *= 2;
    }
    LRUHandle** new_list = new LRUHandle*[new_length];
    memset(new_list, 0, sizeof(new_list[0]) * new_length);
    uint32_t count = 0;
    for (uint32_t i = 0; i < length_; ++i) {
      LRUHandle* h = list_[i];
      while (h != NULL) {
        LRUHandle* next = h->next_hash;
        uint32_t hash = h->hash;
        LRUHandle** ptr = &new_list[hash & (new_length - 1)];
        h->next_hash = *ptr;
        *ptr = h;
        h = next;
        ++count;
      }
    }
    assert(elems_ == count);
    delete[] list_;
    list_ = new_list;
    length_ = new_length;
  }
};

class LRUCache {
 public:
  LRUCache();
  ~LRUCache();
};

}  // namespace cache
}  // namespace voyager
