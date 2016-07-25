#ifndef VOYAGER_CORE_ONLINE_CONNECTIONS_H_
#define VOYAGER_CORE_ONLINE_CONNECTIONS_H_

#include <atomic>
#include <functional>
#include <map>
#include <unordered_map>

#include "voyager/core/tcp_connection.h"
#include "voyager/port/singleton.h"

// 单例类，借助于port::Singleton模板类
namespace voyager {

class OnlineConnections
{
 public:
  OnlineConnections() : size_(0) { }

  int OnlineUserNum(EventLoop* loop) {
    typename std::map<
        EventLoop*, std::unordered_map<std::string, TcpConnectionPtr> >::iterator it;
    it = map_.find(loop);
    if (it != map_.end()) {
      return static_cast<int>((it->second).size());
    } else {
      return 0;
    }
  }

  int AllOnlineUsersNum() const {
    return size_.load(std::memory_order_relaxed);
  }

 private:
  friend class TcpConnection;

  void NewConnection(const TcpConnectionPtr& ptr) {
    EventLoop* loop = ptr->OwnerLoop();
    if (map_.find(loop) != map_.end()) {
      map_[loop][ptr->name()] = ptr;
    } else {
      std::unordered_map<std::string, TcpConnectionPtr> temp;
      temp[ptr->name()] = ptr;
      map_[loop] = std::move(temp);
    }
    ++size_;
  }

  void EraseCnnection(const TcpConnectionPtr& ptr) {
    EventLoop* loop = ptr->OwnerLoop();
    if (map_.find(loop) != map_.end()) {
      assert(map_[loop].find(ptr->name) != map_[loop].end());
      map_[loop].erase(ptr->name());
      --size_;
    }
  }

  std::atomic<int> size_;
  typename std::map<
      EventLoop*, std::unordered_map<std::string, TcpConnectionPtr> > map_;

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
