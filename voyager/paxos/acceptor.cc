#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Acceptor::Acceptor(Config* config)
    : config_(config),
      messager_(config->GetMessager()) {
}

Status Acceptor::Init() {
  uint64_t instance_id = 0;
  Status st = Load(&instance_id);
  if (st.ok()) {
    SetInstanceId(instance_id);
  }

  return st;
}

void Acceptor::OnPrepare(const PaxosMessage& msg) {
  PaxosMessage reply_msg;
  reply_msg.set_message_type(kMsgTypePrepareReply);
  reply_msg.set_instance_id(instance_id_);
  reply_msg.set_node_id(config_->GetNodeId());
  reply_msg.set_proposal_id(msg.proposal_id());

  BallotNumber b(msg.proposal_id(), msg.node_id());

  if (b >= promise_ballot_) {
    reply_msg.set_preaccept_proposal_id(acceptd_ballot_.GetProposalId());
    reply_msg.set_preaccept_node_id(acceptd_ballot_.GetNodeId());
    if (acceptd_ballot_.GetProposalId() > 0) {
      reply_msg.set_value(value_);
    }
    promise_ballot_ =  b;
    Status st = Persist(instance_id_, 0);
    if (!st.ok()) {
    }
  } else {
    reply_msg.set_reject_for_promised_id(acceptd_ballot_.GetProposalId());
  }

  messager_->SendMessage(msg.node_id(), reply_msg);
}

void Acceptor::OnAccpet(const PaxosMessage& msg) {
  PaxosMessage reply_msg;
  reply_msg.set_message_type(kMsgTypeAcceptReply);
  reply_msg.set_instance_id(instance_id_);
  reply_msg.set_node_id(config_->GetNodeId());
  reply_msg.set_proposal_id(msg.proposal_id());

  BallotNumber b(msg.proposal_id(), msg.node_id());
  if (b >= promise_ballot_) {
    promise_ballot_ = b;
    acceptd_ballot_ = b;
    value_ = msg.value();
    Status st = Persist(instance_id_, 0);
    if (!st.ok()) {
    }
  } else {
    reply_msg.set_reject_for_promised_id(promise_ballot_.GetProposalId());
  }

  messager_->SendMessage(msg.node_id(), reply_msg);
}

Status Acceptor::Load(uint64_t* instance_id) {
  return Status::OK();
}

Status Acceptor::Persist(uint64_t instance_id, uint32_t last_checksum) {
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
