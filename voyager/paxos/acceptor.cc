#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/config.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace paxos {

Acceptor::Acceptor(Config* config)
    : log_sync_count_(0),
      config_(config) {
}

bool Acceptor::Init() {
  uint64_t instance_id = 0;
  int res = ReadFromDB(&instance_id);
  if (res != 0) {
    return false;
  }

  SetInstanceId(instance_id);
  VOYAGER_LOG(DEBUG) << "Acceptor::Init - now instance_id_ = " << instance_id_;
  return true;
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
    int ret = WriteToDB(instance_id_, 0);
    if (ret != 0) {
      VOYAGER_LOG(ERROR) << "Acceptor::OnPrepare - write instance_id_ = "
                         << instance_id_ << " to db failed.";
    }
  } else {
    reply_msg.set_reject_for_promise_id(acceptd_ballot_.GetProposalId());
  }

  Messager* messager = config_->GetMessager();
  messager->SendMessage(msg.node_id(), reply_msg);
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
    int ret = WriteToDB(instance_id_, 0);
    if (ret != 0) {
      VOYAGER_LOG(ERROR) << "Acceptor::OnAccpet - write instance_id_ = "
                         << instance_id_ << " to db failed.";
    }

  } else {
    reply_msg.set_reject_for_promise_id(promise_ballot_.GetProposalId());
  }

  Messager* messager = config_->GetMessager();
  messager->SendMessage(msg.node_id(), reply_msg);
}

int Acceptor::ReadFromDB(uint64_t* instance_id) {
  int res = config_->GetDB()->GetMaxInstanceId(instance_id);
  if (res == 1) {
    instance_id = 0;
    return 0;
  }

  std::string value;
  res = config_->GetDB()->Get(*instance_id, &value);
  if (res !=  0) {
    return res;
  }

  AcceptorState state;
  state.ParseFromArray(value.data(), static_cast<int>(value.size()));
  promise_ballot_.SetProposalId(state.promise_id());
  promise_ballot_.SetNodeId(state.promise_node_id());
  acceptd_ballot_.SetProposalId(state.accept_id());
  acceptd_ballot_.SetNodeId(state.accept_node_id());
  return 0;
}

int Acceptor::WriteToDB(uint64_t instance_id, uint32_t last_checksum) {
  AcceptorState state;
  state.set_instance_id(instance_id);
  state.set_promise_id(promise_ballot_.GetProposalId());
  state.set_promise_node_id(promise_ballot_.GetNodeId());
  state.set_accept_id(acceptd_ballot_.GetProposalId());
  state.set_accept_node_id(acceptd_ballot_.GetNodeId());
  state.set_accept_value(value_);
  WriteOptions options;
  options.sync = config_->LogSync();
  if (options.sync) {
    ++log_sync_count_;
    if (log_sync_count_ > config_->SyncInterval()) {
      log_sync_count_ = 0;
    } else {
      options.sync = false;
    }
  }

  std::string value;
  state.SerializeToString(&value);
  int ret = config_->GetDB()->Put(options, instance_id, value);
  return ret;
}

}  // namespace paxos
}  // namespace voyager
