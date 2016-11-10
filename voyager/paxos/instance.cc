#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Instance::Instance(const Config* config)
    : config_(config),
      acceptor_(config),
      learner_(config),
      proposer_(config) {
}

Status Instance::Init() {
  Status st = acceptor_.Init();
  if (!st.ok()) {
    return st;
  }

  uint64_t now_instance_id = acceptor_.GetInstanceId();
  learner_.SetInstanceId(now_instance_id);
  proposer_.SetInstanceId(now_instance_id);
  proposer_.SetStartProposalId(
      acceptor_.GetPromiseBallot().GetProposalId() + 1);
  return Status::OK();
}

Status Instance::OnReceiveMessage(const char* buf, int len) {
  return Status::OK();
}

Status Instance::OnReceive(const std::string& buf) {
  return Status::OK();
}

Status Instance::OnReceivePaxosMessage(const PaxosMessage& msg) {
  switch(msg.message_type()) {
    case PaxosMessage::kPrepareReply:
    case PaxosMessage::kAcceptReply:
      break;
    case PaxosMessage::kPrepare:
    case PaxosMessage::kAccept:
      break;
  }
  return Status::OK();
}

Status Instance::ReceiveMessageForAcceptor(const PaxosMessage& msg) {
  return Status::OK();
}

Status Instance::ReceiveMessageForLearner(const PaxosMessage& msg) {
  return Status::OK();
}

Status Instance::ReceiveMessageForProposer(const PaxosMessage& msg) {
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
