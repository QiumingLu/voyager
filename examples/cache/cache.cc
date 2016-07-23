#include "examples/cache/cache.h"
#include "voyager/port/mutexlock.h"
#include "examples/cache/hash.h"

namespace voyager {
namespace cache {

using port::Mutex;
using port::MutexLock;

struct LRUHandle {
  void* value;
  void (*deleter)(const Slice& key, void* value);
  LRUHandle* next_hash;
  LRUHandle* next;
  LRUHandle* prev;
  size_t charge;
  size_t key_length;
  bool in_cache;    // Whether entry is in the cache.
  uint32_t refs;    // References, including cache reference, if present.
  uint32_t hash;    // Hash of key(); used for fast sharding and comparisons
  char key_data[1]; // Beginning of key

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

// A single shard of sharded cache.
class LRUCache {
 public:
  LRUCache();
  ~LRUCache();

  void SetCapacity(size_t capacity) { capacity_ = capacity; }

  Cache::Handle* Insert(const Slice& key, uint32_t hash,
                        void* value, size_t charge,
                        void (*deleter)(const Slice& key, void* value));
  Cache::Handle* Lookup(const Slice& key, uint32_t hash);
  void Release(Cache::Handle* handle);
  void Erase(const Slice& key, uint32_t hash);
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
};

LRUCache::LRUCache()
    : usage_(0) {
  lru_.next = &lru_;
  lru_.prev = &lru_;
  in_use_.next = &in_use_;
  in_use_.prev = &in_use_;
}

LRUCache::~LRUCache() {
  assert(in_use_.next == &in_use_); // Error if caller has an unreleased handle
  for (LRUHandle* e = lru_.next; e != &lru_; ) {
    LRUHandle* next = e->next;
    assert(e->in_cache);
    e->in_cache = false;
    assert(e->refs == 1);
    UnRef(e);
    e = next;
  }
}

void LRUCache::Ref(LRUHandle* e) {
  if (e->refs == 1 && e->in_cache) { // if in lru list, move to in_use list
    LRU_Remove(e);
    LRU_Append(&in_use_, e);
  }
  e->refs++;
}

void LRUCache::UnRef(LRUHandle* e) {
  assert(e->refs > 0);
  e->refs--;
  if (e->refs == 0) {  // Deallocate
    assert(!e->in_cache);
    (*e->deleter)(e->key(), e->value);
    free(e);
  } else if (e->in_cache && e->refs == 1) { // No longer in use;move to lru_list
    LRU_Remove(e);
    LRU_Append(&lru_, e);
  }
}

void LRUCache::LRU_Remove(LRUHandle* e) {
  e->next->prev = e->prev;
  e->prev->next = e->next;
}

void LRUCache::LRU_Append(LRUHandle* list, LRUHandle* e) {
  // Make "e" newest entry by inserting just before *list
  e->next = list;
  e->prev = list->prev;
  e->prev->next = e;
  e->next->prev = e;
}

Cache::Handle* LRUCache::Lookup(const Slice& key, uint32_t hash) {
  MutexLock lock(&mu_);
  LRUHandle* e= table_.Lookup(key, hash);
  if (e != NULL) {
    Ref(e);
  }
  return reinterpret_cast<Cache::Handle*>(e);
}

void LRUCache::Release(Cache::Handle* handle) {
  MutexLock lock(&mu_);
  UnRef(reinterpret_cast<LRUHandle*>(handle));
}

Cache::Handle* LRUCache::Insert(
    const Slice& key, uint32_t hash, void* value, size_t charge,
    void (*deleter)(const Slice& key, void* value)) {
  MutexLock lock(&mu_);

  LRUHandle* e = reinterpret_cast<LRUHandle*>(
      malloc(sizeof(LRUHandle) - 1 + key.size()));
  e->value = value;
  e->charge = charge;
  e->deleter = deleter;
  e->key_length = key.size();
  e->hash = hash;
  e->in_cache = false;
  e->refs = 1;   // for the returned handle
  memcpy(e->key_data, key.data(), key.size());

  if (capacity_ > 0) {
    e->refs++;  // for the cache's reference.
    e->in_cache = true;
    LRU_Append(&in_use_, e);
    usage_ += charge;
    FinishErase(table_.Insert(e));
  } // else don't cache, Tests use capacity_==0 turning off caching.

  while (usage_ > capacity_ && lru_.next != &lru_) {
    LRUHandle* old = lru_.next;
    assert(old->refs == 1);
    bool erased = FinishErase(table_.Remove(old->key(), old->hash));
    if (!erased) { // to avoid unused variable compiled NDEBUG
      assert(erased);
    }
  }

  return reinterpret_cast<Cache::Handle*>(e);
}

// If e != NULL, finish removing *e from the cache; it has already been removed
// from the hash table. Return whether e != NULL. Requires mu_ held.
bool LRUCache::FinishErase(LRUHandle* e) {
  if (e != NULL) {
    assert(e->in_cache);
    LRU_Remove(e);
    e->in_cache = false;
    usage_ -= e->charge;
    UnRef(e);
  }
  return e != NULL;
}

void LRUCache::Erase(const Slice& key, uint32_t hash) {
  MutexLock lock(&mu_);
  FinishErase(table_.Remove(key, hash));
}

void LRUCache::Purne() {
  MutexLock lock(&mu_);
  while (lru_.next != &lru_) {
    LRUHandle* e = lru_.next;
    assert(e->refs == 1);
    bool erased = FinishErase(table_.Remove(e->key(), e->hash));
    if (!erased) {  // to avoid unused variable while compile NDEBUG
      assert(erased);
    }
  }
}

static const int kNumShardBits = 4;
static const int kNumShards = 1 << kNumShardBits;

class ShardedLRUCache : public Cache {
 public:
  explicit ShardedLRUCache(size_t capacity)
      : last_id_(0) {
    const size_t per_shard = (capacity + (kNumShards - 1) / kNumShards);
    for (int s = 0; s < kNumShards; ++s) {
      shard_[s].SetCapacity(per_shard);
    }
  }

  virtual ~ShardedLRUCache() { }

  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                         void (*deleter)(const Slice& key, void* value)) {
    const uint32_t hash = HashSlice(key);
    return shard_[Shard(hash)].Insert(key, hash, value, charge, deleter);
  }

  virtual Handle* Lookup(const Slice& key) { 
    const uint32_t hash = HashSlice(key);
    return shard_[Shard(hash)].Lookup(key, hash);
  }

  virtual void Release(Handle* handle) {
    LRUHandle* h = reinterpret_cast<LRUHandle*>(handle);
    shard_[Shard(h->hash)].Release(handle);
  }

  virtual void* Value(Handle* handle) { 
    return reinterpret_cast<LRUHandle*>(handle)->value;
  }

  virtual void Erase(const Slice& key) {
    const uint32_t hash = HashSlice(key);
    shard_[Shard(hash)].Erase(key, hash);
  }

  virtual uint64_t NewId() {
    MutexLock lock(&id_mutex_);
    return ++last_id_;
  }

  virtual void Purne() {
    for (int s = 0; s < kNumShards; ++s) {
      shard_[s].Purne();
    }
  }

  virtual size_t TotalCharge() const {
    size_t total = 0;
    for (int s = 0; s < kNumShards; ++s) {
      total += shard_[s].TotalCharge();
    }
    return total;
  }

 private:
  LRUCache shard_[kNumShards];
  Mutex id_mutex_;
  uint64_t last_id_;

  static inline uint32_t HashSlice(const Slice& s) {
    return Hash(s.data(), s.size(), 0);
  }

  static uint32_t Shard(uint32_t hash) {
    return hash >> (32 - kNumShardBits);
  }
};

Cache* NewLRUCache(size_t capacity) {
  return new ShardedLRUCache(capacity);
}

}  // namespace cache
}  // namespace voyager
