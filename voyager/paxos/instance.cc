#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Instance::Instance(Config* config)
    : config_(config),
      acceptor_(config),
      learner_(config),
      proposer_(config),
      loop_(this),
      transfer_(config, &loop_) {
}

Instance::~Instance() {
  loop_.Exit();
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

  loop_.Loop();

  return Status::OK();
}

Status Instance::NewValue(const Slice& value, uint64_t* new_instance_id) {
  return transfer_.NewValue(value, new_instance_id);
}

void Instance::OnReceiveMessage(const Slice& s) {
  loop_.NewMessage(s);
}

void Instance::HandleNewValue(const Slice& value) {
  proposer_.NewValue(value);
}

void Instance::HandleMessage(const std::string& s) {
}

void Instance::HandlePaxosMessage(const PaxosMessage& msg) {
  switch(msg.message_type()) {
    case kMsgTypeProposerSendNewValue:
    case kMsgTypePrepareReply:
    case kMsgTypeAcceptReply:
      ProposerHandleMessage(msg);
      break;
    case kMsgTypePrepare:
    case kMsgTypeAccept:
      AcceptorHandleMessage(msg);
      break;
  }
}

void Instance::AcceptorHandleMessage(const PaxosMessage& msg) {
  if (msg.instance_id() == acceptor_.GetInstanceId() + 1) {
    if (msg.message_type() == kMsgTypePrepare) {
      acceptor_.OnPrepare(msg);
    } else if (msg.message_type() == kMsgTypeAccept) {
      acceptor_.OnAccpet(msg);
    }
  }
}

void Instance::LearnerHandleMessage(const PaxosMessage& msg) {
}

void Instance::ProposerHandleMessage(const PaxosMessage& msg) {
  if (msg.message_type() == kMsgTypePrepareReply) {
    proposer_.OnPrepareReply(msg);
  } else if (msg.message_type() == kMsgTypeAcceptReply) {
    proposer_.OnAccpetReply(msg);
  }
}

}  // namespace paxos
}  // namespace voyager
