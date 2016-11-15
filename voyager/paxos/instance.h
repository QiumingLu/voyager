#ifndef VOYAGER_PAXOS_INSTANCE_H_
#define VOYAGER_PAXOS_INSTANCE_H_

#include <string>

#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/ioloop.h"
#include "voyager/paxos/learner.h"
#include "voyager/paxos/proposer.h"
#include "voyager/paxos/transfer.h"
#include "voyager/util/slice.h"
#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Config;

class Instance {
 public:
  Instance(Config* config);
  ~Instance();

  Status Init();

  Status NewValue(const Slice& value, uint64_t* new_instance_id);

  void OnReceiveMessage(const Slice& s);

  void HandleMessage(const std::string& s);
  void HandlePaxosMessage(const PaxosMessage& msg);

  void AcceptorHandleMessage(const PaxosMessage& msg);
  void LearnerHandleMessage(const PaxosMessage& msg);
  void ProposerHandleMessage(const PaxosMessage& msg);

 private:
  Config* config_;

  Acceptor acceptor_;
  Learner learner_;
  Proposer proposer_;
  Transfer transfer_;
  IOLoop ioloop_;

  // No copying allowed
  Instance(const Instance&);
  void operator=(const Instance&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_INSTANCE_H_
