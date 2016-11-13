#ifndef VOYAGER_PAXOS_INSTANCE_H_
#define VOYAGER_PAXOS_INSTANCE_H_

#include <string>

#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/committer.h"
#include "voyager/paxos/ioloop.h"
#include "voyager/paxos/learner.h"
#include "voyager/paxos/proposer.h"
#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Config;

class Instance {
 public:
  Instance(const Config* config, Messager* messager);
  ~Instance();

  Status Init();

  Status NewValue(const std::string& value, uint64_t* instance_id);

  void OnReceiveMessage(const char* s, size_t n);

  void HandleMessage(const std::string& s);
  void HandlePaxosMessage(const PaxosMessage& msg);

  void AcceptorHandleMessage(const PaxosMessage& msg);
  void LearnerHandleMessage(const PaxosMessage& msg);
  void ProposerHandleMessage(const PaxosMessage& msg);

 private:
  const Config* config_;
  IOLoop ioloop_;

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
