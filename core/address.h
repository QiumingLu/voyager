#ifndef MIRANTS_CORE_ADDRESS_H_
#define MIRANTS_CORE_ADDRESS_H_

namespace mirants {

class SockAddress {
 public:
  SockAddress();

 private:
  union {
    struct sockaddr_in  addr4_;
    struct sockaddr_in6 addr6_;
  };
};

}  // namespace mirants 

#endif  // MIRANTS_CORE_ADDRESS_H_
