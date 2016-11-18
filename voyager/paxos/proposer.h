#ifndef VOYAGER_PAXOS_PROPOSER_H_
#define VOYAGER_PAXOS_PROPOSER_H_

#include <string>
#include <utility>

#include "voyager/paxos/ballot_number.h"
#include "voyager/paxos/counter.h"
#include "voyager/paxos/paxos.pb.h"
#include "voyager/paxos/network/messager.h"
#include "voyager/util/slice.h"

namespace voyager {
namespace paxos {

class Config;

class Proposer {
 public:
  Proposer(Config* config);

  void SetInstanceId(uint64_t id) { instance_id_ = id; }
  void SetStartProposalId(uint64_t id) { proposal_id_ = id; }

  void NewValue(const Slice& value);

  void Prepare(bool need_new_ballot = true);
  void OnPrepareReply(const PaxosMessage& msg);
  void Accept();
  void OnAccpetReply(const PaxosMessage& msg);

  void ExitPrepare();
  void ExitAccept();

 private:
  const Config* config_;
  Messager* messager_;

  BallotNumber hightest_ballot_;
  uint64_t hightest_proprosal_id_;

  uint64_t instance_id_;
  uint64_t proposal_id_;
  std::string value_;

  Counter counter_;

  bool preparing_;
  bool accepting_;
  bool skip_prepare_;
  bool was_rejected_by_someone_;

  // No copying allowed
  Proposer(const Proposer&);
  void operator=(const Proposer&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_PROPOSER_H_
