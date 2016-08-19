#include "voyager/core/connector.h"

#include <assert.h>
#include <string.h>
#include <errno.h>

#include "voyager/core/eventloop.h"
#include "voyager/core/tcp_client.h"
#include "voyager/util/stringprintf.h"
#include "voyager/util/logging.h"

namespace voyager {

Connector::Connector(EventLoop* ev, const SockAddr& addr) 
    : ev_(CHECK_NOTNULL(ev)),
      addr_(addr),
      state_(kDisConnected),
      retry_time_(kInitRetryTime),
      connect_(false),
      dispatch_(),
      socket_() {
}

void Connector::Start() {
  ConnectorPtr ptr(shared_from_this());
  ev_->RunInLoop([ptr]() {
    ptr->connect_ = true;
    ptr->StartInLoop();
  });
}

void Connector::StartInLoop() {
  ev_->AssertInMyLoop();
  assert(this->state_ == kDisconnected);
  if (connect_) {
    Connect();
  }
}

void Connector::Stop() {
  ConnectorPtr ptr(shared_from_this());
  ev_->QueueInLoop([ptr]() {
    ptr->connect_ = false;
    if (ptr->state_ == kConnecting) {
      ptr->state_ = kDisConnected;
      ptr->ResetDispatch();
    }
  });
}

void Connector::Connect() {
  socket_.reset(new ClientSocket(addr_.Family(), true));
  int ret = socket_->Connect(addr_.GetSockAddr(),
                             sizeof(*(addr_.GetSockAddr())));
  int err = (ret == 0) ? 0 : errno;
  switch (err) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
      Connecting();
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      Retry();
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
    case ENAMETOOLONG:
    
    default:
      VOYAGER_LOG(ERROR) << "connect: " << strerror(err);
      socket_.reset();
      break;
  }
}

void Connector::Connecting() {
  state_ = kConnecting; 
  dispatch_.reset(new Dispatch(ev_, socket_->SocketFd()));
  dispatch_->Tie(shared_from_this());
  dispatch_->SetWriteCallback(std::bind(&Connector::ConnectCallback, this));
  dispatch_->EnableWrite();
}

void Connector::Retry() {
  state_ = kDisConnected;
  if (connect_) {
    VOYAGER_LOG(INFO) << "Connector::Retry - Retry connecting to "
                      << addr_.Ipbuf() << " in " << retry_time_ 
                      << " seconds.";

    ConnectorPtr ptr(shared_from_this());
#ifdef __linux__
    if (!timer_.get()) {
      timer_.reset(new NewTimer(ev_, [ptr]() { ptr->StartInLoop(); }));
    }
    timer_->SetTime(retry_time_ * 1000, 0);
#else
    ev_->RunAfter(retry_time_, [ptr]() {ptr->StartInLoop();});
#endif

    retry_time_ = 
        (retry_time_*2) < kMaxRetryTime ? retry_time_*2 : kMaxRetryTime;
  }
}

void Connector::ConnectCallback() {
  if (state_ == kConnecting) {
    ResetDispatch();
    Status st = socket_->CheckSocketError();
    if (!st.ok()) {
      VOYAGER_LOG(WARN) << st;
      Retry();
    } else if (socket_->IsSelfConnect() == 0) {
      VOYAGER_LOG(WARN) << "Connector::ConnectCallback - Self connect";
      Retry();
    } else {
      state_ = kConnected;
      VOYAGER_LOG(INFO) << "Connector::ConnectCallback - " 
                        << StateToString();
      if (connect_ && newconnection_cb_) {
        socket_->SetNoAutoCloseFd();
        newconnection_cb_(socket_->SocketFd());
      }
    }
    socket_.reset();
  }
}

std::string Connector::StateToString() const {
  const char* type;
  switch (state_) {
    case kDisConnected:
      type = "DisConnected";
      break;
    case kConnected:
      type = "Connected";
      break;
    case kConnecting:
      type = "Connecting";
      break;
    default:
      type = "Unknown State";
      break;
  }
  std::string result(type);
  return result;
}

void Connector::ResetDispatch() {
  if (dispatch_.get()) {
    dispatch_->DisableAll();
    dispatch_->RemoveEvents();
    ConnectorPtr ptr(shared_from_this());
    ev_->QueueInLoop([ptr]() {
      ptr->dispatch_.reset();
    });
  }
}

}  // namespace voyager
