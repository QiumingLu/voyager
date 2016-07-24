#ifndef VOYAGER_CORE_ONLINE_CONNECTIONS_H_
#define VOYAGER_CORE_ONLINE_CONNECTIONS_H_

#include "voyager/port/mutexlock.h"
#include "voyager/port/singleton.h"
#include <unordered_map>

namespace voyager {

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

  // No Copying allow
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
  
  // No copying allow
  ConcurrentMap(const ConcurrentMap&);
  void operator=(const ConcurrentMap&);
};

// 单例类，借助于port::Singleton模板类
class OnlineConnections
{
 public:
  OnlineConnections() { }

  void NewConnection(const std::string& name, const TcpConnectionPtr& ptr) {
    map_.Insert(name, ptr);
  }

  void EraseCnnection(const TcpConnectionPtr& ptr) {
    map_.Erase(ptr->name());
  }

  size_t OnlineUsersNum() const {
    return map_.size();
  }

 private:
  ConcurrentMap<std::string, TcpConnectionPtr> map_;

  OnlineConnections(const OnlineConnections&);
  void operator=(const OnlineConnections&);	
};

}  // namespace voyager

#endif  // VOYAGER_CORE_ONLINE_CONNECTIONS_H_
