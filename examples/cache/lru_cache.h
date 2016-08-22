#ifndef EXAMPLES_CACHE_LRU_CACHE_H_
#define EXAMPLES_CACHE_LRU_CACHE_H_

#include <stdint.h>

#include <functional>
#include <string>

#include "voyager/port/mutexlock.h"
#include "voyager/util/slice.h"

namespace voyager {
namespace cache {

using port::Mutex;
using port::MutexLock;

struct LRUHandle {
 private:
  friend class HandleTable;
  friend class LRUCache;
  friend class ShardedLRUCache;

  void* value;
  void (*deleter)(const Slice& key, void* value);
  LRUHandle* next_hash;
  LRUHandle* next;
  LRUHandle* prev;
  size_t charge;
  size_t key_length;
  size_t hash;       // Hash of key(); used for fast sharding and comparisons
  uint32_t refs;     // References, including cache reference, if present.
  bool in_cache;     // Whether entry is in the cache.
  char key_data[1];  // Beginning of key

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

  LRUHandle* Lookup(const Slice& key, size_t hash);
  LRUHandle* Insert(LRUHandle* h);
  LRUHandle* Remove(const Slice& key, size_t hash);

 private:
  LRUHandle** FindPointer(const Slice& key, size_t hash);
  void Resize();

  uint32_t length_;
  uint32_t elems_;
  LRUHandle** list_;
};

// A single shard of sharded cache.
class LRUCache {
 public:
  LRUCache();
  ~LRUCache();

  void SetCapacity(size_t capacity) { capacity_ = capacity; }

  LRUHandle* Insert(const Slice& key, size_t hash,
                        void* value, size_t charge,
                        void (*deleter)(const Slice& key, void* value));
  LRUHandle* Lookup(const Slice& key, size_t hash);
  void Release(LRUHandle* handle);
  void Erase(const Slice& key, size_t hash);
  void Purne();
  size_t TotalCharge() const {
    MutexLock lock(&mu_);
    return usage_;
  }

 private:
  void LRU_Remove(LRUHandle* e);
  void LRU_Append(LRUHandle* lsit, LRUHandle* e);
  void Ref(LRUHandle* e);
  void UnRef(LRUHandle* e);
  bool FinishErase(LRUHandle* e);

  // Initialized before use
  size_t capacity_;

  // mu_ protectes the following state.
  mutable Mutex mu_;
  size_t usage_;

  // Dummy head of LRU list.
  // lru.prev is newest entry, lru.next is oldest entry.
  // Entries have refs==1 and in_cache==true.
  LRUHandle lru_;

  // Dummy head of in-use list.
  // Entries are in use by clients, and have refs >= 2 and in_cache==true.
  LRUHandle in_use_;

  HandleTable table_;

  // No copying allow
  LRUCache(const LRUCache&);
  void operator=(const LRUCache&);
};

static const int kNumShardBits = 4;
static const int kNumShards = 1 << kNumShardBits;

class ShardedLRUCache {
 public:
  explicit ShardedLRUCache(size_t capacity);
  ~ShardedLRUCache() { }

  // Insert a mapping from key->value into the cache and assign it
  // the specified charge against the total cache capacity.
  //
  // Returns a handle taht correspondings to the mapping. The caller
  // must call this->Release(handle) when the returned mapping is no
  // longer needed.
  //
  // When the inserted entry is no longer needed, the key and value will
  // the passed to "deleter".
  LRUHandle* Insert(const Slice& key, void* value, size_t charge,
                         void (*deleter)(const Slice& key, void* value));

  // If the cache has no mapping for ""key", returns NULL
  //
  // Else return a handle that corresponds to the mapping.
  // The caller call this->Release(handle) when the returned mapping
  // is no longer needed.
  LRUHandle* Lookup(const Slice& key);

  // Release a mapping returned by a previous Lookup().
  // REQUIRES: handle must not have been released yet.
  // REQUIRES: handle must have been returned by a method on *this.
  void Release(LRUHandle* handle);

  // Return the value encapsulated in a handle returned by a successful
  // Lookup().
  // REQUIRES: handle must not have been released yet.
  // REQUIRES: handle must have returned by a method on *this.
  void* Value(LRUHandle* handle);

  // If the cache contains entry for key, erase it. Note that the
  // underlying entry will be kept around until all existing handles
  // to it have been released.
  void Erase(const Slice& key);

  // Return a new numberic id. May be used by multiple clients who are
  // sharing the same cache to partition the key space. Typically the
  // client will allocate a new id at startup and prepend the id to
  // it cache keys.
  uint64_t NewId();

  // Remove all cache entries taht are not activity in use.
  // Memory-constrained applications may wish to call this method to
  // reduce memory usage.
  // Default implementation of Prune() does nothing. Subclasses are
  // strongly encouraged to override the default implementation.
  void Purne();

  // Return an estimate of the combined charges of all elements stored in
  // the cache.
  size_t TotalCharge() const;

 private:
  static inline size_t HashSlice(const Slice& s) {
    std::hash<std::string> h;
    return h(s.ToString());
  }

  static uint32_t Shard(size_t hash) {
    return (hash & (kNumShards - 1));
  }

  void LRU_Remove(LRUHandle* e);
  void LRU_Append(LRUHandle* e);
  void UnRef(LRUHandle* e);

  LRUCache shard_[kNumShards];
  Mutex id_mutex_;
  uint64_t last_id_;

  // No copying allow
  ShardedLRUCache(const ShardedLRUCache&);
  void operator=(const ShardedLRUCache&);
};

}  // namespace cache
}  // namespace voyager

#endif  // EXAMPLES_CACHE_LRU_CACHE_H_
