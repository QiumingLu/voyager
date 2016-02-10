#include "core/tcp_connection.h"
#include "core/dispatch.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "core/socket_util.h"
#include "util/logging.h"

namespace mirants {

TcpConnection::TcpConnection(const std::string& name, EventLoop* ev, int fd,
                             const SockAddr& local_addr,
                             const struct sockaddr_storage& peer_sa)
    : name_(name), 
      eventloop_(CHECK_NOTNULL(ev)),
      socket_(fd),
      state_(kConnecting),
      dispatch_(new Dispatch(ev, fd)),
      local_addr_(local_addr),
      peer_sa_(peer_sa) {
  dispatch_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
  dispatch_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
  dispatch_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
  dispatch_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
  socket_.SetKeepAlive(true);
  MIRANTS_LOG(DEBUG) << "TcpConnection::TcpConnection [" << name_ << "] at "
                     << this << " fd=" << fd;
}
 
TcpConnection::~TcpConnection() {
  MIRANTS_LOG(DEBUG) << "TcpConnection::~TcpConnection [" << name_ << "] at "
                     << this << " fd=" << dispatch_->Fd()
                     << " ConnectState=" << StateToString();
  assert(state_ == kDisconnected);
}

void TcpConnection::EstablishConnection() {
  eventloop_->AssertThreadSafe();
  assert(state_ == kConnecting);
  state_ = kConnected;
  dispatch_->Tie(shared_from_this());
  dispatch_->EnableRead();
  if (connection_cb_) {
    connection_cb_(shared_from_this());
  }
}

void TcpConnection::DeleteConnection() {
  eventloop_->AssertThreadSafe();
  if (state_ == kConnected) {
    state_ = kDisconnected;
    dispatch_->DisableAll();
  }
  dispatch_->RemoveEvents();
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
  assert(state_ == kConnected || state_ == kDisconnecting);
  state_ = kDisconnected;
  dispatch_->DisableAll();
  close_cb_(shared_from_this());
}

void TcpConnection::HandleError() {
  Status st = sockets::CheckSocketError(dispatch_->Fd());
  MIRANTS_LOG(ERROR) << "TcpConnection::HandleError [" << name_
                     << "] - " << st.ToString();
}

void TcpConnection::SendMessage(std::string&& message) {
  if (state_ == kConnected) {
    if (eventloop_->IsInCreatedThread()) {
      SendInLoop(&*message.begin(), message.size());
    } else {
      eventloop_->RunInLoop(
        std::bind(&TcpConnection::SendInLoop, 
                  this,
                  &*message.begin(), 
                  message.size())); 
    }
  }
}

void TcpConnection::SendMessage(const Slice& message) {
  if (state_ == kConnected) {
    if (eventloop_->IsInCreatedThread()) {
      SendMessageInLoop(message);
    } else {
      eventloop_->RunInLoop(
        std::bind(&TcpConnection::SendMessageInLoop, this, message.ToString()));
    }
  }
}

void TcpConnection::SendMessage(Buffer* message) {
  // if (state_ == kConnected) {
  //   if (eventloop_->IsInCreatedThread()) {
  //     SendInLoop(message->Peek(), message->ReadableSize());
  //     message->RetrieveAll();
  //   } else {
  //     eventloop_->RunInLoop(
  //       std::bind(&TcpConnection::SendInLoop, this, message->RetrieveAllString()));
  //   }
  // }
}

void TcpConnection::SendMessageInLoop(const Slice& message) {
  SendInLoop(message.data(), message.size());
}

void TcpConnection::SendInLoop(const void* data, size_t size) {
  eventloop_->AssertThreadSafe();
  if (state_ == kDisconnected) {
    return;
  }
  ssize_t nwrote = 0;
  size_t  remaining = size;
  bool fault = false;

  if (!dispatch_->IsWriting() && writebuf_.ReadableSize() == 0) {
    nwrote = sockets::Write(dispatch_->Fd(), data, size);
    if (nwrote >= 0) {
      remaining = size - nwrote;
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        MIRANTS_LOG(ERROR) << "TcpConnection::SendMessageInLoop [" << name_ 
                           << "] - " << strerror(errno);
        if (errno == EPIPE || errno == ECONNRESET) {
          fault = true;
        }
      }
    }
  }

  assert(remaining <= size);
  // FIXME
  if (!fault && remaining > 0) {
    if (!dispatch_->IsWriting()) {
      dispatch_->EnableWrite();
      SendInLoop(static_cast<const char*>(data)+nwrote, remaining);
    }
  }
}

std::string TcpConnection::StateToString() const {
  const char *type;
  switch (state_) {
    case kDisconnected:
      type = "Disconnected";
      break;
    case kDisconnecting:
      type = "Disconnecting";
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

}  // namespace mirants
