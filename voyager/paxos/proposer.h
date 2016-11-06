#ifndef VOYAGER_PAXOS_PROPOSER_H_
#define VOYAGER_PAXOS_PROPOSER_H_

#include "voyager/paxos/counter.h"
#include "voyager/paxos/paxos_message.h"

namespace voyager {
namespace paxos {

class Config;

class Proposer {
 public:
  Proposer(const Config* config);

  void Prepare();
  void OnPrepareReply(const PaxosMessage& msg);
  void Accept();
  void OnAccpetReply(const PaxosMessage& msg);

 private:
  Counter counter_;

  // No copying allowed
  Proposer(const Proposer&);
  void operator=(const Proposer&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_PROPOSER_H_
