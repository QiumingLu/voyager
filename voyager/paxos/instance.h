#ifndef VOYAGER_PAXOS_INSTANCE_H_
#define VOYAGER_PAXOS_INSTANCE_H_

#include <string>

#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/committer.h"
#include "voyager/paxos/learner.h"
#include "voyager/paxos/proposer.h"
#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Config;

class Instance {
 public:
  Instance(const Config* config);

  Status Init();

  Status OnReceiveMessage(const char* buf, int len);
  Status OnReceive(const std::string& buf);
  Status OnReceivePaxosMessage(const PaxosMessage& msg);

  Status ReceiveMessageForAcceptor(const PaxosMessage& msg);
  Status ReceiveMessageForLearner(const PaxosMessage& msg);
  Status ReceiveMessageForProposer(const PaxosMessage& msg);

 private:
  const Config* config_;
  Acceptor acceptor_;
  Learner learner_;
  Proposer proposer_;
  Committer committer_;

  // No copying allowed
  Instance(const Instance&);
  void operator=(const Instance&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_INSTANCE_H_
