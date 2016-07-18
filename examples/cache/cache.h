#ifndef EXAMPLES_CACHE_CACHE_H_
#define EXAMPLES_CACHE_CACHE_H_

#include "voyager/util/slice.h"

namespace voyager {
namespace cache {

class Cache {
 public:
  Cache() { }
  virtual ~Cache() { }

  struct Handle { };

  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                         void (*deleter)(const Slice& key, void* value));

  virtual Handle* Lookup(const Slice& key) = 0;
 
  virtual void Release(Handle* handle) = 0;

  virtual void* Value(Handle* handle) = 0;

  virtual void Erase(const Slice& key) = 0;

 private:

  // No copying allow
  Cache(const Cache&);
  void operator=(const Cache&);
};

} // namespace cache
} // namespace voyager

#endif // EXAMPLES_CACHE_CACHE_H_
