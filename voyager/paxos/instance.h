#ifndef VOYAGER_PAXOS_INSTANCE_H_
#define VOYAGER_PAXOS_INSTANCE_H_

#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/learner.h"
#include "voyager/paxos/proposer.h"

namespace voyager {
namespace paxos {

class Config;

class Instance {
 public:
  Instance(const Config* config);

 private:
  const Config* config_;
  Acceptor acceptor_;
  Learner learner_;
  Proposer proposer_;

  // No copying allowed
  Instance(const Instance&);
  void operator=(const Instance&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_INSTANCE_H_
