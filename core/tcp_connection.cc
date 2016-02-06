#include "core/tcp_connection.h"
#include "core/dispatch.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "util/logging.h"

namespace mirants {

TcpConnection::TcpConnection(const std::string& name, EventLoop* ev, int fd,
                             const SockAddr& local_addr,
                             struct sockaddr_storage* peer_sa)
    : name_(name), 
      eventloop_(ev),
      state_(kConnecting),
      socket_(fd),
      dispatch_(new Dispatch(ev, fd)),
      local_addr_(local_addr),
      peer_sa_(peer_sa) {
  dispatch_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
  dispatch_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
  dispatch_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
  dispatch_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
  socket_.SetKeepAlive(true);
}
 
TcpConnection::~TcpConnection() {
}

void TcpConnection::EstablishConnection() {
  eventloop_->AssertThreadSafe();
  assert(state_ == kConnecting);
  state_ = kConnected;
  dispatch_->EnableRead();
  if (connection_cb_) {
    connection_cb_(shared_from_this());
  }
}

void TcpConnection::DeleteConnection() {
  
}

void TcpConnection::HandleRead() {
  eventloop_->AssertThreadSafe();
  int err;
  ssize_t n = readbuf_.ReadV(dispatch_->Fd(), err);
  if (n > 0) {
    if (message_cb_) {
      message_cb_(shared_from_this(), &readbuf_);
    }
  } else if (n == 0) {
    HandleClose();
  } else {
    errno = err;
    MIRANTS_LOG(ERROR) << "TcpConnection::HandleRead - " << strerror(errno);
    HandleError();
  }
}

void TcpConnection::HandleWrite() {
}

void TcpConnection::HandleClose() {
  eventloop_->AssertThreadSafe();
  MIRANTS_LOG(TRACE) << "TcpConnection::HandleClose";
  dispatch_->DisableAll();
}

void TcpConnection::HandleError() {
}

}  // namespace mirants
