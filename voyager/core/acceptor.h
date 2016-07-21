#ifndef VOYAGER_CORE_ACCEPTOR_H_
#define VOYAGER_CORE_ACCEPTOR_H_

#include <functional>
#include <netdb.h>
#include "voyager/core/tcp_socket.h"
#include "voyager/core/dispatch.h"

namespace voyager {

class SockAddr;
class EventLoop;
class Acceptor {
 public:
  typedef std::function<void (int fd, 
      const struct sockaddr_storage& sa)> NewConnectionCallback;

  Acceptor(EventLoop* eventloop, 
           const SockAddr& addr, 
           int backlog, bool reuseport = false);
  ~Acceptor();

  void EnableListen();
  bool IsListenning() const { return listenning_; }

  void SetNewConnectionCallback(const NewConnectionCallback& func) { 
    connfunc_ = func; 
  }
  void SetNewConnectionCallback(NewConnectionCallback&& func) { 
    connfunc_ = std::move(func); 
  }

 private:
  void Accept();
 
  EventLoop* eventloop_;
  TcpSocket  tcpsocket_;
  Dispatch   dispatch_;
  int        backlog_;
  int        idlefd_;
  bool       listenning_;
  NewConnectionCallback connfunc_;

  // No copying allow
  Acceptor(const Acceptor&);
  void operator=(const Acceptor&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_ACCEPTOR_H_
