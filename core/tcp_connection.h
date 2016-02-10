#ifndef MIRANTS_CORE_TCP_CONNECTION_H_
#define MIRANTS_CORE_TCP_CONNECTION_H_

#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include "core/buffer.h"
#include "core/callback.h"
#include "core/sockaddr.h"
#include "core/tcp_socket.h"
#include "util/scoped_ptr.h"

namespace mirants {

class Dispatch;
class EventLoop;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(const std::string& name, EventLoop* ev, int fd,
                const SockAddr& local_addr,
                const struct sockaddr_storage& peer_sa);
  ~TcpConnection();

  void SetConnectionCallback(const ConnectionCallback& func) { 
    connection_cb_ = func; 
  }
  void SetConnectionCallback(ConnectionCallback&& func) {
    connection_cb_ = std::move(func);
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& func) {
    writecompletet_cb_ = func;
  }
  void SetWriteCompleteCallback(WriteCompleteCallback&& func) {
    writecompletet_cb_ = std::move(func);
  }
  void SetMessageCallback(const MessageCallback& func) {
    message_cb_ = func;
  }
  void SetMessageCallback(MessageCallback&& func) {
    message_cb_ = std::move(func);
  }
  void SetCloseCallback(const CloseCallback& func) {
    close_cb_ = func;
  }
  void SetCloseCallback(CloseCallback&& func) {
    close_cb_ = std::move(func);
  }

  EventLoop* GetLoop() const { return eventloop_; }
  const std::string& name() const { return name_; }

  void EstablishConnection();
  void DeleteConnection();

  std::string StateToString() const;

  void SendMessage(std::string&& message);
  void SendMessage(const Slice& message);
  void SendMessage(Buffer* message);

 private:
  enum ConnectState {
    kDisconnected,
    kDisconnecting,
    kConnected,
    kConnecting
  };

  void SendInLoop(const void* data, size_t size);

  void HandleRead();
  void HandleWrite();
  void HandleClose();
  void HandleError();

  const std::string name_;
  EventLoop* eventloop_;
  TcpSocket socket_;
  ConnectState state_;
  scoped_ptr<Dispatch> dispatch_;
  const SockAddr local_addr_;
  struct sockaddr_storage peer_sa_;

  Buffer readbuf_;
  Buffer writebuf_;


  ConnectionCallback connection_cb_;
  WriteCompleteCallback writecompletet_cb_;
  MessageCallback message_cb_;
  CloseCallback close_cb_;
  
  // No copying allow
  TcpConnection(const TcpConnection&);
  void operator=(const TcpConnection&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_CONNECTION_H_
