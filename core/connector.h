#ifndef MIRANTS_CORE_CONNECTOR_H_
#define MIRANTS_CORE_CONNECTOR_H_

#include <netdb.h>

namespace mirants {

class Connector {
 public:
  Connector(const struct addrinfo* addr);

 private:
  void TcpNonBlockConnect();

  const struct addrinfo* servinfo_;

  // No copying allow
  Connector(const Connector&);
  void operator=(const Connector&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_CONNECTOR_H_
