#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Instance::Instance(const Config* config, Messager* messager)
    : config_(config),
      ioloop_(this),
      acceptor_(config, messager),
      learner_(config, messager),
      proposer_(config, messager) {
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

Status Instance::NewValue(const std::string& value, uint64_t* instance_id) {
  proposer_.NewValue(value);
  return Status::OK();
}

void Instance::OnReceiveMessage(const char* s, size_t n) {
  ioloop_.NewMessage(s, n);
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
