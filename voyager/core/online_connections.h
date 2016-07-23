#ifndef VOYAGER_CORE_ONLINE_CONNECTIONS_H_
#define VOYAGER_CORE_ONLINE_CONNECTIONS_H_

#include "voyager/port/mutexlock.h"
#include "voyager/port/singleton.h"
#include <unordered_map>

namespace voyager {

// 单例类，借助于port::Singleton模板类
class OnlineConnections
{
 public:
  OnlineConnections() : mu_(), conn_map_() { }
  ~OnlineConnections() { }

  void NewConnection(const std::string& name, const TcpConnectionPtr& ptr) {
    port::MutexLock lock(&mu_);
    conn_map_[name] = ptr;
  }

  void EraseCnnection(const TcpConnectionPtr& ptr) {
    port::MutexLock lock(&mu_);
    conn_map_.erase(ptr->name());
  }

  size_t OnlineUsersNum() const {
    port::MutexLock lock(&mu_);
    return conn_map_.size();
  }

 private:
  mutable port::Mutex mu_;
  std::unordered_map<std::string, TcpConnectionPtr> conn_map_;

  OnlineConnections(const OnlineConnections&);
  void operator=(const OnlineConnections&);	
};

}  // namespace voyager

#endif  // VOYAGER_CORE_ONLINE_CONNECTIONS_H_
