#ifndef VOYAGER_PORT_CONCURRENT_MAP_H_
#define VOYAGER_PORT_CONCURRENT_MAP_H_

#include <unordered_map>

#include "voyager/port/mutexlock.h"

namespace voyager {
namespace port {

static const int kNumShardBits = 4;
static const int kNumShards = 1 << kNumShardBits;

template <typename K, typename V>
class HashMap {
 public:
  HashMap() { }

  void Insert(const K& key, const V& value) {
    port::MutexLock lock(&mu_);
    map_[key] = value;
  }

  void Erase(const K& key) {
    port::MutexLock lock(&mu_);
    map_.erase(key);
  }

  size_t size() const {
    port::MutexLock lock(&mu_);
    return  map_.size();
  }

 private:
  mutable port::Mutex mu_;
  typename std::unordered_map<K, V> map_;

  // No copying allowed
  HashMap(const HashMap&);
  void operator=(const HashMap&);
};

template <typename K, typename V>
class ConcurrentMap {
 public:
  ConcurrentMap() { }

  void Insert(const K& key, const V& value) {
    const size_t h = Hash(key);
    shard_[Shard(h)].Insert(key, value);
  }

  void Erase(const K& key) {
    const size_t h = Hash(key);
    shard_[Shard(h)].Erase(key);
  }

  size_t size() const {
    size_t total = 0;
    for (int s = 0; s < kNumShards; ++s) {
      total += shard_[s].size();
    }
    return total;
  }

 private:
  static inline size_t Hash(const K& key) {
    typename std::hash<K> h;
    return h(key);
  }

  static inline size_t Shard(size_t h) {
    return (h & (kNumShards - 1));
  }

  HashMap<K, V> shard_[kNumShards];

  // No copying allowed
  ConcurrentMap(const ConcurrentMap&);
  void operator=(const ConcurrentMap&);
};

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_CONCURRENT_MAP_H_
