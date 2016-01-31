#include "core/tcp_client.h"
#include "core/connector.h"
#include "core/sockaddr.h"
#include "util/logging.h"

namespace mirants {

TcpClient::TcpClient(const SockAddr& addr)
    : servinfo_(addr.AddrInfo()),
      connector_ptr_(new Connector(servinfo_)) {
}

TcpClient::~TcpClient() { }

void TcpClient::TcpConnect() {
  MIRANTS_LOG(INFO) << "TcpClient::TcpConnect[" << "]" << " connecting to ";
   //                 << servinfo_;
  connector_ptr_->TcpNonBlockConnect();
}

}  // namespace mirants