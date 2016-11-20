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

bool Instance::Init() {
  bool ret = acceptor_.Init();
  if (!ret) {
    return ret;
  }
  uint64_t now_instance_id = acceptor_.GetInstanceId();
  learner_.SetInstanceId(now_instance_id);
  proposer_.SetInstanceId(now_instance_id);
  proposer_.SetStartProposalId(
      acceptor_.GetPromiseBallot().GetProposalId() + 1);

  loop_.Loop();

  return ret;
}

bool Instance::NewValue(const Slice& value, uint64_t* new_instance_id) {
  return transfer_.NewValue(value, new_instance_id);
}

void Instance::OnReceiveMessage(const Slice& s) {
  loop_.NewMessage(s);
}

void Instance::HandleNewValue(const Slice& value) {
  proposer_.NewValue(value);
}

void Instance::HandleMessage(const std::string& s) {
  PaxosMessage msg;
  msg.ParseFromArray(s.data(), static_cast<int>(s.size()));
  HandlePaxosMessage(msg);
}

void Instance::HandlePaxosMessage(const PaxosMessage& msg) {
  switch(msg.type()) {
    case PROPOSER_SEND_NEW_VALUE:
    case PREPARE_REPLY:
    case ACCEPT_REPLY:
      ProposerHandleMessage(msg);
      break;
    case PREPARE:
    case ACCEPT:
      AcceptorHandleMessage(msg);
      break;
    default:
      break;
  }
}

void Instance::AcceptorHandleMessage(const PaxosMessage& msg) {
  if (msg.instance_id() == acceptor_.GetInstanceId() + 1) {
    if (msg.type() == PREPARE) {
      acceptor_.OnPrepare(msg);
    } else if (msg.type() == ACCEPT) {
      acceptor_.OnAccpet(msg);
    }
  }
}

void Instance::LearnerHandleMessage(const PaxosMessage& msg) {
}

void Instance::ProposerHandleMessage(const PaxosMessage& msg) {
  if (msg.type() == PREPARE_REPLY) {
    proposer_.OnPrepareReply(msg);
  } else if (msg.type() == ACCEPT_REPLY) {
    proposer_.OnAccpetReply(msg);
  }
}

}  // namespace paxos
}  // namespace voyager
