#ifndef VOYAGER_CORE_CONNECTOR_H_
#define VOYAGER_CORE_CONNECTOR_H_

#include <functional>
#include <memory>
#include <string>
#include <netdb.h>

#include "voyager/core/sockaddr.h"
#include "voyager/util/scoped_ptr.h"

namespace voyager {

class Dispatch;
class EventLoop;

class Connector : public std::enable_shared_from_this<Connector> {
 public:
  typedef std::function<void (int fd)> NewConnectionCallback;

  Connector(EventLoop* ev, const SockAddr& addr);

  void SetNewConnectionCallback(const NewConnectionCallback& func) {
    newconnection_cb_ = func;
  }

  void Start();
  void ReStart();
  void Stop();

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

  void ConnectCallback();
  void HandleError();

  int DeleteOldDispatch();

  std::string StateToString() const;

  EventLoop* ev_;
  SockAddr addr_;
  ConnectState state_;
  double retry_time_;
  bool connect_;
  scoped_ptr<Dispatch> dispatch_;
  NewConnectionCallback newconnection_cb_;

  // No copying allow
  Connector(const Connector&);
  void operator=(const Connector&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_CONNECTOR_H_
