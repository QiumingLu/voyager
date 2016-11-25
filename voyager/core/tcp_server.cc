#include "voyager/core/tcp_server.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/schedule.h"
#include "voyager/core/tcp_acceptor.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"

namespace voyager {

TcpServer::TcpServer(EventLoop* ev,
                     const SockAddr& addr,
                     const std::string& name,
                     int thread_size,
                     int backlog)
    : eventloop_(CHECK_NOTNULL(ev)),
      ipbuf_(addr.Ipbuf()),
      name_(name),
      acceptor_(new TcpAcceptor(eventloop_, addr, backlog)),
      schedule_(new Schedule(eventloop_, thread_size-1)),
      conn_id_(0) {
  acceptor_->SetNewConnectionCallback(
      std::bind(&TcpServer::NewConnection, this,
                std::placeholders::_1, std::placeholders::_2));
  VOYAGER_LOG(INFO) << "TcpServer::TcpServer [" << name_ << "] is running";
}

TcpServer::~TcpServer() {
  VOYAGER_LOG(INFO) << "TcpServer::~TcpServer [" << name_ << "] is down";
}

void TcpServer::Start() {
  eventloop_->RunInLoop([this]() {
    if (seq_.GetNext() == 0) {
      schedule_->Start();
      assert(!acceptor_->IsListenning());
      acceptor_->EnableListen();
    }
  });
}

void TcpServer::NewConnection(int fd, const struct sockaddr_storage& sa) {
  eventloop_->AssertInMyLoop();
  char peer[64];
  SockAddr::SockAddrToIPPort(reinterpret_cast<const sockaddr*>(&sa),
                             peer, sizeof(peer));
  std::string conn_name = StringPrintf("%s-%s#%d",
                                       ipbuf_.c_str(), peer, ++conn_id_);

  VOYAGER_LOG(INFO) << "TcpServer::NewConnection [" << name_
                    << "] - new connection [" << conn_name
                    << "] from " << peer;

  EventLoop* ev = schedule_->AssignLoop();
  TcpConnectionPtr conn_ptr(new TcpConnection(conn_name, ev, fd));

  conn_ptr->SetConnectionCallback(connection_cb_);
  conn_ptr->SetCloseCallback(close_cb_);
  conn_ptr->SetWriteCompleteCallback(writecomplete_cb_);
  conn_ptr->SetMessageCallback(message_cb_);

  ev->RunInLoop([conn_ptr]() {
    conn_ptr->StartWorking();
  });
}

}  // namespace voyager
