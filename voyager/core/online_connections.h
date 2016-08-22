#ifndef VOYAGER_CORE_ONLINE_CONNECTIONS_H_
#define VOYAGER_CORE_ONLINE_CONNECTIONS_H_

#include <map>
#include <string>
#include <unordered_map>
#include <utility>

#include "voyager/core/tcp_connection.h"
#include "voyager/port/singleton.h"
#include "voyager/port/mutexlock.h"
#include "voyager/util/stl_util.h"

namespace voyager {

class OneLoopConnections {
 public:
  OneLoopConnections() { }
  ~OneLoopConnections() { }

  void Insert(const TcpConnectionPtr& ptr) {
    connections_[ptr->name()] = ptr;
  }

  void Erase(const TcpConnectionPtr& ptr) {
    connections_.erase(ptr->name());
  }

  size_t Size() const {
    return connections_.size();
  }

 private:
  typedef std::unordered_map<std::string, TcpConnectionPtr> ConnectionMap;

  ConnectionMap connections_;

  OneLoopConnections(const OneLoopConnections&);
  void operator=(const OneLoopConnections&);
};

// 单例类，借助于port::Singleton模板类

class OnlineConnections {
 public:
  OnlineConnections() : mu_() { }
  ~OnlineConnections() { STLDeleteValues(&loop_map_); }

  size_t OnlineUserNum(EventLoop* loop) {
    port::MutexLock lock(&mu_);
    if (loop_map_.find(loop) != loop_map_.end()) {
      return loop_map_[loop]->Size();
    }
    return 0;
  }

  size_t AllOnlineUsersNum() const {
    port::MutexLock lock(&mu_);
    size_t total = 0;
    for (LoopMap::const_iterator it = loop_map_.begin();
         it != loop_map_.end(); ++it) {
      total += it->second->Size();
    }
    return total;
  }

  // 供Loop析构的时候调用，此函数将会强行关闭所有运行在该Loop上的连接，
  void Erase(EventLoop* loop) {
    port::MutexLock lock(&mu_);
    LoopMap::iterator it = loop_map_.find(loop);
    if (it != loop_map_.end()) {
      delete it->second;
      loop_map_.erase(it);
    }
  }

  // in voyager, insert and erase are in the same loop
  void NewConnection(const TcpConnectionPtr& ptr) {
    port::MutexLock lock(&mu_);
    EventLoop* loop = ptr->OwnerLoop();
    loop->AssertInMyLoop();
    if (loop_map_.find(loop) != loop_map_.end()) {
      loop_map_[loop]->Insert(ptr);
    } else {
      OneLoopConnections* temp = new OneLoopConnections();
      temp->Insert(ptr);
      loop_map_[loop] = temp;
    }
  }


  void EraseCnnection(const TcpConnectionPtr& ptr) {
    port::MutexLock lock(&mu_);
    EventLoop* loop = ptr->OwnerLoop();
    loop->AssertInMyLoop();
    if (loop_map_.find(loop) != loop_map_.end()) {
      loop_map_[loop]->Erase(ptr);
    }
  }

 private:
  typedef std::map<EventLoop*, OneLoopConnections*> LoopMap;

  mutable port::Mutex mu_;
  LoopMap loop_map_;;

  // No copying allow
  OnlineConnections(const OnlineConnections&);
  void operator=(const OnlineConnections&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_ONLINE_CONNECTIONS_H_
