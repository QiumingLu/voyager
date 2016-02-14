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

void TcpConnection::ShutDown() {
  if (state_ == kConnected) {
    state_ = kDisconnecting;
    eventloop_->RunInLoop(std::bind(&TcpConnection::ShutDownInLoop, this));
  }
}

void TcpConnection::ShutDownInLoop() {
  eventloop_->AssertThreadSafe();
  if (!dispatch_->IsWriting()) {
    socket_.ShutDownWrite();
  }
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
    MIRANTS_LOG(ERROR) << "TcpConnection::HandleRead [" << name_ 
                       <<"] - readv: " << strerror(errno);
  }
}

void TcpConnection::HandleWrite() {
  eventloop_->AssertThreadSafe();
  if (dispatch_->IsWriting()) {
    ssize_t n = sockets::Write(dispatch_->Fd(), 
                               writebuf_.Peek(), 
                               writebuf_.ReadableSize());
    int err = errno;
    if (n >= 0) {
      writebuf_.Retrieve(n);
      if (writebuf_.ReadableSize() == 0) {
        dispatch_->DisableWrite();
        if (writecomplete_cb_) {
          eventloop_->QueueInLoop(
              std::bind(writecomplete_cb_, shared_from_this()));
        }
        if (state_ == kDisconnecting) {
          ShutDownInLoop();
        }
      }
    } else {
      MIRANTS_LOG(ERROR) << "TcpConnection::HandleWrite [" << name_ 
                         << "] - write: " << strerror(err);
    }
  } else {
    MIRANTS_LOG(TRACE) << "TcpConnection::HandleWrite [" << name_ << "] - fd="
                       << dispatch_->Fd() << " is down, no more writing";
  }
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
  if (!st.ok()) {
    MIRANTS_LOG(ERROR) << "TcpConnection::HandleError [" << name_
                       << "] - " << st.ToString();
  }
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
      SendInLoop(message.data(), message.size());
    } else {
      std::string s(message.ToString());
      eventloop_->RunInLoop(
          std::bind(&TcpConnection::SendInLoop, this, &*s.begin(), s.size()));
    }
  }
}

void TcpConnection::SendMessage(Buffer* message) {
  CHECK_NOTNULL(message);
  if (state_ == kConnected) {
    if (eventloop_->IsInCreatedThread()) {
      SendInLoop(message->Peek(), message->ReadableSize());
      message->RetrieveAll();
    } else {
      std::string s(message->RetrieveAllAsString());
      eventloop_->RunInLoop(
          std::bind(&TcpConnection::SendInLoop, this, &*s.begin(), s.size()));
    }
  }
}

void TcpConnection::SendInLoop(const void* data, size_t size) {
  eventloop_->AssertThreadSafe();
  if (state_ == kDisconnected) {
    MIRANTS_LOG(WARN) << "TcpConnection::SendInLoop[" << name_ << "]"
                      << "has disconnected, give up writing.";
    return;
  }

  ssize_t nwrote = 0;
  size_t  remaining = size;
  bool fault = false;

  if (!dispatch_->IsWriting() && writebuf_.ReadableSize() == 0) {
    nwrote = sockets::Write(dispatch_->Fd(), data, size);
    if (nwrote >= 0) {
      remaining = size - nwrote;
      if (remaining == 0 && writecomplete_cb_) {
        eventloop_->QueueInLoop(
            std::bind(writecomplete_cb_, shared_from_this()));
      }
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        MIRANTS_LOG(ERROR) << "TcpConnection::SendInLoop [" << name_ 
                           << "] - write: " << strerror(errno);
        if (errno == EPIPE || errno == ECONNRESET) {
          fault = true;
        }
      }
    }
  }

  assert(remaining <= size);
  if (!fault && remaining > 0) {
    writebuf_.Append(static_cast<const char*>(data)+nwrote, remaining);
    if (!dispatch_->IsWriting()) {
      dispatch_->EnableWrite();
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
