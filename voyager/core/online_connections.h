#ifndef VOYAGER_CORE_ONLINE_CONNECTIONS_H_
#define VOYAGER_CORE_ONLINE_CONNECTIONS_H_

#include <map>
#include <unordered_map>

#include "voyager/core/tcp_connection.h"
#include "voyager/port/singleton.h"
#include "voyager/port/mutex.h"
#include "voyager/util/stl_util.h"

// 单例类，借助于port::Singleton模板类
namespace voyager {

class OneLoopConnections {
 public:
  OneLoopConnections() :mu_() { }

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
  mutable port::Mutex mu_;
  std::unordered_map<std::string, TcpConnectionPtr> connections_;

  OneLoopConnections(const OneLoopConnections&);
  void operator=(const OneLoopConnections&);
};

class OnlineConnections
{
 public:
  OnlineConnections() { }
  ~OnlineConnections() { STLDeleteValues(&m_); }

  size_t OnlineUserNum(EventLoop* loop) {
    if (m_.find(loop) != m_.end()) {
      return m_[loop]->Size();
    }
    return 0;
  }

  size_t AllOnlineUsersNum() const {
    size_t total = 0;
    std::map<EventLoop*, OneLoopConnections*>::const_iterator it;
    for (it = m_.begin(); it != m_.end(); ++it) {
      total += it->second->Size();
    }
    return total;
  }

 private:
  friend class TcpConnection;

  // in voyager, insert and erase are in the same loop
  void NewConnection(const TcpConnectionPtr& ptr) {
    EventLoop* loop = ptr->OwnerLoop();
    if (m_.find(loop) != m_.end()) {
      m_[loop]->Insert(ptr);
    } else {
      OneLoopConnections* temp = new OneLoopConnections();
      temp->Insert(ptr);
      m_[loop] = temp;
    }
  }

  void EraseCnnection(const TcpConnectionPtr& ptr) {
    EventLoop* loop = ptr->OwnerLoop();
    if (m_.find(loop) != m_.end()) {
      m_[loop]->Erase(ptr);
    }
  }

  std::map<EventLoop*, OneLoopConnections*> m_;

  OnlineConnections(const OnlineConnections&);
  void operator=(const OnlineConnections&); 
};

}  // namespace voyager

// #include "voyager/port/concurrent_map"

// class OnlineConnections
// {
//  public:
//   OnlineConnections() { }

//   void NewConnection(const std::string& name, const TcpConnectionPtr& ptr) {
//     map_.Insert(name, ptr);
//   }

//   void EraseCnnection(const TcpConnectionPtr& ptr) {
//     map_.Erase(ptr->name());
//   }

//   size_t OnlineUsersNum() const {
//     return map_.size();
//   }

//  private:
//   port::ConcurrentMap<std::string, TcpConnectionPtr> map_;

//   OnlineConnections(const OnlineConnections&);
//   void operator=(const OnlineConnections&);	
// };

#endif  // VOYAGER_CORE_ONLINE_CONNECTIONS_H_
