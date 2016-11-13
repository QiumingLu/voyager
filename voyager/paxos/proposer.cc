#include "voyager/paxos/proposer.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Proposer::Proposer(Config* config)
    : config_(config),
      messager_(config->GetMessager()),
      hightest_proprosal_id_(0),
      instance_id_(0),
      proposal_id_(1),
      counter_(config),
      preparing_(false),
      accepting_(false),
      skip_prepare_(false),
      was_rejected_by_someone_(false) {
}

void Proposer::NewValue(const std::string& value) {
  if (value_.size() == 0) {
    value_ = value;
  }

  if (skip_prepare_ && !was_rejected_by_someone_) {
    Accept();
  } else {
    Prepare(was_rejected_by_someone_);
  }
}

void Proposer::Prepare(bool need_new_ballot) {
  ExitAccept();
  preparing_ = true;
  skip_prepare_ = false;
  was_rejected_by_someone_ = false;

  hightest_ballot_.Reset();

  if (need_new_ballot) {
    if (proposal_id_ < hightest_proprosal_id_) {
      proposal_id_ = hightest_proprosal_id_;
    }
    proposal_id_ += 1;
  }

  PaxosMessage msg;
  msg.set_message_type(PaxosMessage::kPrepare);
  msg.set_instance_id(instance_id_);
  msg.set_node_id(config_->GetNodeId());
  msg.set_proposal_id(proposal_id_);

  counter_.StartNewRound();

  messager_->BroadcastMessage(msg);
}

void Proposer::OnPrepareReply(const PaxosMessage& msg) {
  if (preparing_) {
    if (msg.proposal_id() == proposal_id_) {
      counter_.AddReceivedNode(msg.node_id());

      if (msg.reject_for_promised_id() == 0) {
        counter_.AddPromisorOrAcceptor(msg.node_id());
        BallotNumber b(msg.preaccept_proposal_id(), msg.preaccept_node_id());
        if (b > hightest_ballot_) {
          hightest_ballot_ = b;
          value_ = msg.value();
        }
      } else {
        counter_.AddRejector(msg.node_id());
        was_rejected_by_someone_ = true;
        if (hightest_proprosal_id_ < msg.reject_for_promised_id()) {
          hightest_proprosal_id_ = msg.reject_for_promised_id();
        }
      }

      if (counter_.IsPassedOnThisRound()) {
        skip_prepare_ = true;
        Accept();
      } else if (counter_.IsRejectedOnThisRound() ||
                 counter_.IsReceiveAllOnThisRound()) {
      }
    }
  }
}

void Proposer::Accept() {
  ExitPrepare();
  accepting_ = true;

  PaxosMessage msg;
  msg.set_message_type(PaxosMessage::kAccept);
  msg.set_instance_id(instance_id_);
  msg.set_node_id(config_->GetNodeId());
  msg.set_proposal_id(proposal_id_);
  msg.set_value(value_);

  counter_.StartNewRound();
  messager_->BroadcastMessage(msg);
}

void Proposer::OnAccpetReply(const PaxosMessage& msg) {
  if (accepting_) {
    if (msg.proposal_id() == proposal_id_) {
      counter_.AddReceivedNode(msg.node_id());

      if (msg.reject_for_promised_id() == 0) {
        counter_.AddPromisorOrAcceptor(msg.node_id());
      } else {
        counter_.AddRejector(msg.node_id());
        was_rejected_by_someone_ = true;
        if (hightest_proprosal_id_ < msg.reject_for_promised_id()) {
          hightest_proprosal_id_ = msg.reject_for_promised_id();
        }
      }

      if (counter_.IsPassedOnThisRound()) {
        ExitAccept();
      } else if (counter_.IsRejectedOnThisRound() ||
                 counter_.IsReceiveAllOnThisRound()) {
      }
    }
  }
}

void Proposer::ExitPrepare() {
  if (preparing_) {
    preparing_ = false;
  }
}

void Proposer::ExitAccept() {
  if (accepting_) {
    accepting_ = false;
  }
}

}  // namespace paxos
}  // namespace voyager
