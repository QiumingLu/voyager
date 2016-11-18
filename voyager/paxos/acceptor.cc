#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/config.h"
#include "voyager/paxos/paxos.pb.h"

namespace voyager {
namespace paxos {

Acceptor::Acceptor(Config* config)
    : config_(config),
      messager_(config->GetMessager()) {
}

void Acceptor::Init() {
  uint64_t instance_id = 0;
  int res = ReadFromDB(&instance_id);
  if (res != 0) {
    SetInstanceId(instance_id);
  }
}

void Acceptor::OnPrepare(const PaxosMessage& msg) {
  PaxosMessage reply_msg;
  reply_msg.set_type(PREPARE_REPLY);
  reply_msg.set_instance_id(instance_id_);
  reply_msg.set_node_id(config_->GetNodeId());
  reply_msg.set_proposal_id(msg.proposal_id());

  BallotNumber b(msg.proposal_id(), msg.node_id());

  if (b >= promise_ballot_) {
    reply_msg.set_pre_accept_id(acceptd_ballot_.GetProposalId());
    reply_msg.set_pre_accept_node_id(acceptd_ballot_.GetNodeId());
    if (acceptd_ballot_.GetProposalId() > 0) {
      reply_msg.set_value(value_);
    }
    promise_ballot_ =  b;
    WriteToDB(instance_id_, 0);
  } else {
    reply_msg.set_reject_for_promise_id(acceptd_ballot_.GetProposalId());
  }

  messager_->SendMessage(msg.node_id(), reply_msg);
}

void Acceptor::OnAccpet(const PaxosMessage& msg) {
  PaxosMessage reply_msg;
  reply_msg.set_type(ACCEPT_REPLY);
  reply_msg.set_instance_id(instance_id_);
  reply_msg.set_node_id(config_->GetNodeId());
  reply_msg.set_proposal_id(msg.proposal_id());

  BallotNumber b(msg.proposal_id(), msg.node_id());
  if (b >= promise_ballot_) {
    promise_ballot_ = b;
    acceptd_ballot_ = b;
    value_ = msg.value();
    WriteToDB(instance_id_, 0);
  } else {
    reply_msg.set_reject_for_promise_id(promise_ballot_.GetProposalId());
  }

  messager_->SendMessage(msg.node_id(), reply_msg);
}

int Acceptor::ReadFromDB(uint64_t* instance_id) {
  int res = config_->GetDB()->GetMaxInstanceId(instance_id);
  if (res != 0 && res != 1) {
    return res;
  }
  if (res == 1) {
    instance_id = 0;
    return 0;
  }

  std::string value;
  res = config_->GetDB()->Get(*instance_id, &value);
  if (res !=  0) {
    return res;
  }
  AcceptorStateMessage msg;
  msg.ParseFromArray(value.data(), static_cast<int>(value.size()));

  return 0;
}

void Acceptor::WriteToDB(uint64_t instance_id, uint32_t last_checksum) {
  AcceptorStateMessage msg;
  msg.set_instance_id(instance_id);
  msg.set_promise_id(promise_ballot_.GetProposalId());
  msg.set_promise_node_id(promise_ballot_.GetNodeId());
  msg.set_accept_id(acceptd_ballot_.GetProposalId());
  msg.set_accept_node_id(acceptd_ballot_.GetNodeId());
  msg.set_accept_value(value_);
  WriteOptions options;
  options.sync = config_->LogSync();
  if (options.sync) {

  }

  std::string value;
  msg.SerializeToString(&value);
  int res = config_->GetDB()->Put(options, instance_id, value);
  if (res != 0) {
  }
}

}  // namespace paxos
}  // namespace voyager
