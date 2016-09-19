#ifndef VOYAGER_CORE_ONLINE_CONNECTIONS_H_
#define VOYAGER_CORE_ONLINE_CONNECTIONS_H_

#include <map>
#include <string>
#include <unordered_map>

#include "voyager/core/tcp_connection.h"
#include "voyager/port/singleton.h"
#include "voyager/port/mutexlock.h"

namespace voyager {

class OneLoopConnections {
 public:
  OneLoopConnections() : mu_() { }

  void Insert(const TcpConnectionPtr& ptr) {
    port::MutexLock lock(&mu_);
    connections_[ptr->name()] = ptr;
  }

  void Erase(const TcpConnectionPtr& ptr) {
    port::MutexLock lock(&mu_);
    connections_.erase(ptr->name());
  }

  size_t Size() const {
    port::MutexLock lock(&mu_);
    return connections_.size();
  }

 private:
  typedef std::unordered_map<std::string, TcpConnectionPtr> ConnectionMap;

  mutable port::Mutex mu_;
  ConnectionMap connections_;

  // No copying alloweded
  OneLoopConnections(const OneLoopConnections&);
  void operator=(const OneLoopConnections&);
};

// 单例类，借助于port::Singleton模板类

class OnlineConnections {
 public:
  OnlineConnections() { }

  EventLoop* GetLoop() const {
    LoopMap::const_iterator it = loop_map_.begin();
    assert(it != loop_map_.end());
    EventLoop* loop = it->first;
    size_t min = it->second->Size();
    ++it;
    while (it != loop_map_.end()) {
      if (it->second->Size() < min) {
        loop = it->first;
      }
      ++it;
    }
    assert(loop != nullptr);
    return loop;
  }

  void Insert(EventLoop* loop) {
    assert(loop_map_.find(loop) == loop_map_.end());
    loop_map_[loop] =
        std::unique_ptr<OneLoopConnections>(new OneLoopConnections());
  }

  void Clear() {
    loop_map_.clear();
  }

  // in voyager, insert and erase are in the same loop
  void NewConnection(const TcpConnectionPtr& ptr) {
    EventLoop* loop = ptr->OwnerLoop();
    loop->AssertInMyLoop();
    assert(loop_map_.find(loop) != loop_map_.end());
    loop_map_[loop]->Insert(ptr);
  }

  void EraseCnnection(const TcpConnectionPtr& ptr) {
    EventLoop* loop = ptr->OwnerLoop();
    loop->AssertInMyLoop();
    assert(loop_map_.find(loop) != loop_map_.end());
    loop_map_[loop]->Erase(ptr);
  }

 private:
  typedef std::map<EventLoop*, std::unique_ptr<OneLoopConnections>> LoopMap;
  LoopMap loop_map_;;

  // No copying alloweded
  OnlineConnections(const OnlineConnections&);
  void operator=(const OnlineConnections&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_ONLINE_CONNECTIONS_H_
