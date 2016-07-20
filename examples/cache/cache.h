#ifndef EXAMPLES_CACHE_CACHE_H_
#define EXAMPLES_CACHE_CACHE_H_

#include <stdint.h>
#include "voyager/util/slice.h"

namespace voyager {
namespace cache {

class Cache;

extern Cache* NewLRUCache(size_t capacity);

class Cache {
 public:
  Cache() { }
  virtual ~Cache();

  struct Handle { };

  // Insert a mapping from key->value into the cache and assign it
  // the specified charge against the total cache capacity.
  //
  // Returns a handle taht correspondings to the mapping. The caller
  // must call this->Release(handle) when the returned mapping is no
  // longer needed.
  //
  // When the inserted entry is no longer needed, the key and value will
  // the passed to "deleter".
  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                         void (*deleter)(const Slice& key, void* value));

  // If the cache has no mapping for ""key", returns NULL
  //
  // Else return a handle that corresponds to the mapping.
  // The caller call this->Release(handle) when the returned mapping
  // is no longer needed.
  virtual Handle* Lookup(const Slice& key) = 0;
 
  // Release a mapping returned by a previous Lookup().
  // REQUIRES: handle must not have been released yet.
  // REQUIRES: handle must have been returned by a method on *this.
  virtual void Release(Handle* handle) = 0;

  // Return the value encapsulated in a handle returned by a successful
  // Lookup().
  // REQUIRES: handle must not have been released yet.
  // REQUIRES: handle must have returned by a method on *this.
  virtual void* Value(Handle* handle) = 0;

  // If the cache contains entry for key, erase it. Note that the
  // underlying entry will be kept around until all existing handles
  // to it have been released.
  virtual void Erase(const Slice& key) = 0;

  // Return a new numberic id. May be used by multiple clients who are
  // sharing the same cache to partition the key space. Typically the
  // client will allocate a new id at startup and prepend the id to 
  // it cache keys.
  virtual uint64_t NewId() = 0;

  // Remove all cache entries taht are not activity in use. 
  // Memory-constrained applications may wish to call this method to
  // reduce memory usage.
  // Default implementation of Prune() does nothing. Subclasses are 
  // strongly encouraged to override the default implementation.
  virtual void Purne() { }

  // Return an estimate of the combined charges of all elements stored in 
  // the cache.
  virtual size_t TotalCharge() const = 0;

 private:
  void LRU_Remove(Handle* e);
  void LRU_Append(Handle* e);
  void UnRef(Handle* e);

  struct Request;
  Request* req_;

  // No copying allow
  Cache(const Cache&);
  void operator=(const Cache&);
};

} // namespace cache
} // namespace voyager

#endif // EXAMPLES_CACHE_CACHE_H_
