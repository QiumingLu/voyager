#ifndef VOYAGER_CORE_TCP_SOCKET_H_
#define VOYAGER_CORE_TCP_SOCKET_H_

#include <netinet/in.h>

namespace voyager {

class TcpSocket {
 public:
  explicit TcpSocket(int socketfd) : socketfd_(socketfd) { }
  ~TcpSocket();

  int SocketFd() const { return socketfd_; }

  void BindAddress(const struct sockaddr* sa, socklen_t salen);
  void Listen(int backlog);
  int Accept(struct sockaddr* peer_sa, socklen_t* salen);
  
  void SetReuseAddr(bool on);
  void SetReusePort(bool on);
  void SetKeepAlive(bool on);
  void SetTcpNoDelay(bool on);

  void ShutDownWrite();
  
 private:
  const int socketfd_;

  // No copying allow
  TcpSocket(const TcpSocket&);
  void operator=(const TcpSocket&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_SOCKET_H_
