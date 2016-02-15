#ifndef MIRANTS_CORE_CONNECTOR_H_
#define MIRANTS_CORE_CONNECTOR_H_

#include <functional>
#include <memory>
#include <string>
#include <netdb.h>

#include "core/sockaddr.h"
#include "util/scoped_ptr.h"

namespace mirants {

class Dispatch;
class EventLoop;

class Connector : public std::enable_shared_from_this<Connector> {
 public:
  typedef std::function<void (int fd)> NewConnectionCallback;

  Connector(EventLoop* ev, const SockAddr& addr);

  void Start();
  void Stop();

  SockAddr ServerAddr() const { return addr_; }
  std::string StateToString() const;

 private:
  enum ConnectState {
    kDisConnected,
    kConnected,
    kConnecting
  };

  static const double kMaxRetryTime;
  static const double kInitRetryTime;

  void StartInLoop();
  void StopInLoop();

  void Connect();
  void Connecting(int socketfd);
  void Retry(int socketfd);

  void HandleWrite();
  void HandleError();

  int DeleteOldDispatch();
  void ResetDispatch();

  EventLoop* ev_;
  SockAddr addr_;
  ConnectState state_;
  double retry_time_;
  bool connect_;
  scoped_ptr<Dispatch> dispatch_;

  // No copying allow
  Connector(const Connector&);
  void operator=(const Connector&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_CONNECTOR_H_
