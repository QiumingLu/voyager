#include "tcp_client.h"
#include "connector.h"
#include "sockaddr.h"

namespace mirants {

TcpClient::TcpClient(const SockAddr& addr)
    : servinfo_(addr.AddInfo()),
    : connector_ptr_(new Connector(servinfo_)) {
}

TcpClient::TcpConnect() {
  connector_ptr_->TcpNonBlockConnect();
}
