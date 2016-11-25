#include "voyager/paxos/learner.h"
#include "voyager/paxos/config.h"
#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Learner::Learner(Config* config, Instance* instance, Acceptor* acceptor)
    : config_(config),
      instance_(instance),
      acceptor_(acceptor),
      instance_id_(0),
      has_learned_(false) {
}

void Learner::OnNewChosenValue(const PaxosMessage& msg) {
  if (msg.instance_id() != instance_id_) {
    return;
  }
  const BallotNumber& b = acceptor_->GetAcceptedBallot();
  BallotNumber ballot(msg.proposal_id(), msg.node_id());
  if (ballot == b) {
    VOYAGER_LOG(DEBUG) << "Learner::OnNewChosenValue - learn a new chosen value"
                       << ", now instance_id_=" << instance_id_
                       << ", learned_value_=" << learned_value_;
    instance_id_ = acceptor_->GetInstanceId();
    learned_value_ = acceptor_->GetAcceptedValue();
    has_learned_ = true;
    BroadcastMessageToFollower();
    VOYAGER_LOG(DEBUG) << "Learner::OnNewChosenValue - learn a new chosen value,"
                       << " which node_id=" << msg.node_id()
                       << ", proposal_id=" << msg.proposal_id()
                       << ", and now learn's instance_id_=" << instance_id_
                       << ", learned_value_=" << learned_value_;

  }
}

void Learner::AskForLearn() {
  PaxosMessage* msg = new PaxosMessage();
  msg->set_node_id(config_->GetNodeId());
  msg->set_instance_id(instance_id_);
  msg->set_type(LEARNER_ASK_FOR_LEARN);
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->BroadcastMessage(msg);
}

void Learner::OnAskForLearn(const PaxosMessage& msg) {
  if (msg.instance_id() >= instance_id_) {
    return;
  }
  SendNowInstanceId(msg);
}

void Learner::SendNowInstanceId(const PaxosMessage& msg) {
  PaxosMessage* reply_msg = new PaxosMessage();
  reply_msg->set_instance_id(msg.instance_id());
  reply_msg->set_node_id(config_->GetNodeId());
  reply_msg->set_type(LEARNER_SEND_NOW_INSTANCE_ID);
  reply_msg->set_now_instance_id(instance_id_);
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->SendMessage(msg.node_id(), reply_msg);
}

void Learner::OnSendNowInstanceId(const PaxosMessage& msg) {
  if (msg.instance_id() != instance_id_) {
    return;
  }
  if (msg.now_instance_id() <= instance_id_) {
    return;
  }

  ComfirmAskForLearn(msg);
}

void Learner::ComfirmAskForLearn(const PaxosMessage& msg) {
  PaxosMessage* reply_msg = new PaxosMessage();
  reply_msg->set_node_id(config_->GetNodeId());
  reply_msg->set_instance_id(instance_id_);
  reply_msg->set_type(LEARNER_COMFIRM_ASK_FOR_LEARN);
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->SendMessage(msg.node_id(), reply_msg);
}

void Learner::OnComfirmAskForLearn(const PaxosMessage& msg) {
}

void Learner::BroadcastMessageToFollower() {
  PaxosMessage* msg = new PaxosMessage();
  msg->set_type(LEARNER_SEND_LEARNED_VALUE);
  msg->set_node_id(config_->GetNodeId());
  msg->set_instance_id(instance_id_);
  msg->set_proposal_node_id(acceptor_->GetAcceptedBallot().GetNodeId());
  msg->set_proposal_id(acceptor_->GetAcceptedBallot().GetProposalId());
  msg->set_value(acceptor_->GetAcceptedValue());
  assert(config_->GetMessager() != nullptr);
  config_->GetMessager()->BroadcastMessageToFollower(msg);
}

void Learner::NextInstance() {
  ++instance_id_;
  has_learned_ = false;
  learned_value_.clear();
}

}  // namespace paxos
}  // namespace voyager
