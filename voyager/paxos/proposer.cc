#include "voyager/paxos/proposer.h"
#include "voyager/paxos/config.h"
#include "voyager/paxos/instance.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace paxos {

Proposer::Proposer(Config* config, Instance* instance)
    : config_(config),
      instance_(instance),
      hightest_proprosal_id_(0),
      instance_id_(0),
      proposal_id_(1),
      counter_(config),
      preparing_(false),
      accepting_(false),
      skip_prepare_(false),
      was_rejected_by_someone_(false) {
}

void Proposer::NewValue(const Slice& value) {
  if (value_.size() == 0) {
    value_ = value.ToString();
  }

  if (skip_prepare_ && !was_rejected_by_someone_) {
    Accept();
  } else {
    Prepare(was_rejected_by_someone_);
  }
}

void Proposer::Prepare(bool need_new_ballot) {
  preparing_ = true;
  accepting_ = false;
  skip_prepare_ = false;
  was_rejected_by_someone_ = false;

  hightest_ballot_.Reset();

  if (need_new_ballot) {
    if (proposal_id_ < hightest_proprosal_id_) {
      proposal_id_ = hightest_proprosal_id_;
    }
    proposal_id_ += 1;
  }

  PaxosMessage* msg = new PaxosMessage();
  msg->set_type(PREPARE);
  msg->set_node_id(config_->GetNodeId());
  msg->set_instance_id(instance_id_);
  msg->set_proposal_id(proposal_id_);

  counter_.StartNewRound();

  VOYAGER_LOG(DEBUG) << "Proposer::Prepare - start a new prepare, now "
                     << "node_id=" << config_->GetNodeId()
                     << ", instance_id=" << instance_id_
                     << ", proposal_id_=" << proposal_id_
                     << ", value=" << value_;
  instance_->HandlePaxosMessage(*msg);
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->BroadcastMessage(msg);
}

void Proposer::OnPrepareReply(const PaxosMessage& msg) {
  if (preparing_) {
    if (msg.proposal_id() == proposal_id_) {
      counter_.AddReceivedNode(msg.node_id());

      if (msg.reject_for_promised_id() == 0) {
        counter_.AddPromisorOrAcceptor(msg.node_id());
        BallotNumber b(msg.pre_accepted_id(), msg.pre_accepted_node_id());
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
        VOYAGER_LOG(DEBUG) << "Proposer::OnPrepareReply - Prepare not pass,"
                           << "reprepare 300ms later.";
      }
    }
  }
}

void Proposer::Accept() {
  preparing_ = false;
  accepting_ = true;

  PaxosMessage* msg = new PaxosMessage();
  msg->set_type(ACCEPT);
  msg->set_node_id(config_->GetNodeId());
  msg->set_instance_id(instance_id_);
  msg->set_proposal_id(proposal_id_);
  msg->set_value(value_);

  counter_.StartNewRound();
  instance_->HandlePaxosMessage(*msg);
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->BroadcastMessage(msg);
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
        accepting_ = false;
        NewChosenValue();
      } else if (counter_.IsRejectedOnThisRound() ||
                 counter_.IsReceiveAllOnThisRound()) {
        VOYAGER_LOG(DEBUG) << "Proposer::OnAccpetReply - Accept not pass,"
                           << "reprepare 300ms later.";
      }
    }
  }
}

void Proposer::NewChosenValue() {
  PaxosMessage* msg = new PaxosMessage();
  msg->set_type(NEW_CHOSEN_VALUE);
  msg->set_node_id(config_->GetNodeId());
  msg->set_instance_id(instance_id_);
  msg->set_proposal_id(proposal_id_);
  instance_->HandlePaxosMessage(*msg);
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->BroadcastMessage(msg);
}

void Proposer::NextInstance() {
  hightest_proprosal_id_ = 0;
  value_.clear();
  preparing_ = false;
  accepting_ = false;
  ++instance_id_;
  counter_.StartNewRound();
}

}  // namespace paxos
}  // namespace voyager
