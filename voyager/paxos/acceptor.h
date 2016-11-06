#ifndef VOYAGER_PAXOS_ACCEPTOR_H_
#define VOYAGER_PAXOS_ACCEPTOR_H_

#include "voyager/paxos/paxos_message.h"

namespace voyager {
namespace paxos {

class Config;

class Acceptor {
 public:
  Acceptor(const Config* config);

  void OnPrepare(const PaxosMessage& msg);
  void OnAccpet(const PaxosMessage& msg);

 private:

  // No copying allowed
  Acceptor(const Acceptor&);
  void operator=(const Acceptor&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_ACCEPTOR_H_
