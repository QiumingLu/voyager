#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Instance::Instance(Config* config)
    : config_(config),
      acceptor_(config),
      learner_(config),
      proposer_(config),
      ioloop_(this),
      transfer_(config, &ioloop_) {
}

Instance::~Instance() {
  ioloop_.Exit();
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

  ioloop_.Loop();

  return Status::OK();
}

Status Instance::NewValue(const Slice& value, uint64_t* new_instance_id) {
  return transfer_.NewValue(value, new_instance_id);
}

void Instance::OnReceiveMessage(const Slice& s) {
  ioloop_.NewMessage(s);
}

void Instance::HandleNewValue(const Slice& value) {
  proposer_.NewValue(value);
}

void Instance::HandleMessage(const std::string& s) {
}

void Instance::HandlePaxosMessage(const PaxosMessage& msg) {
  switch(msg.message_type()) {
    case PaxosMessage::kPrepareReply:
    case PaxosMessage::kAcceptReply:
      break;
    case PaxosMessage::kPrepare:
    case PaxosMessage::kAccept:
      break;
  }
}

void Instance::AcceptorHandleMessage(const PaxosMessage& msg) {
}

void Instance::LearnerHandleMessage(const PaxosMessage& msg) {
}

void Instance::ProposerHandleMessage(const PaxosMessage& msg) {
}

}  // namespace paxos
}  // namespace voyager
