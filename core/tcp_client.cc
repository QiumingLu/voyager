#include "core/tcp_client.h"
#include "core/connector.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "util/logging.h"

namespace mirants {

TcpClient::TcpClient(EventLoop* ev, const SockAddr& addr)
    : ev_(CHECK_NOTNULL(ev)),
      connector_ptr_(new Connector(ev, addr)) {
}

TcpClient::~TcpClient() { }

void TcpClient::TcpConnect() {
  MIRANTS_LOG(INFO) << "TcpClient::TcpConnect - connecting to "
                    << connector_ptr_->ServerAddr().IP();
  connector_ptr_->Start();
}

}  // namespace mirants
