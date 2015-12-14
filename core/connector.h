#ifndef MIRANTS_CORE_CONNECTOR_H_
#define MIRANTS_CORE_CONNECTOR_H_

namespace mirants {

class Connector {
 public:
  Connector();

 private:
  // No copying allow
  Connector(const Connector&);
  void operator=(const Connector&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_CONNECTOR_H_
