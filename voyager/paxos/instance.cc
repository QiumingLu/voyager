#include "voyager/paxos/instance.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace paxos {

Instance::Instance(Config* config)
    : config_(config),
      acceptor_(config),
      learner_(config, &acceptor_),
      proposer_(config),
      loop_(this),
      transfer_(config, &loop_) {
  proposer_.SetChosenValueCallback([this](uint64_t i_id, uint64_t p_id) {
    learner_.NewChosenValue(i_id, p_id);
  });
  learner_.SetNewChosenValueCallback([this](const PaxosMessage& msg) {
    LearnerHandleMessage(msg);
  });
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
      acceptor_.GetPromisedBallot().GetProposalId() + 1);

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

void Instance::HandleMessage(const Content& content) {
  if (content.type() == PAXOS_MESSAGE) {
    HandlePaxosMessage(content.paxos_msg());
  } else if (content.type() == CHECKPOINT_MESSAGE) {
    HandleCheckPointMessage(content.checkpoint_msg());
  }
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
      LearnerHandleMessage(msg);
      break;
  }
}

void Instance::HandleCheckPointMessage(const CheckPointMessage& msg) {
}

void Instance::ProposerHandleMessage(const PaxosMessage& msg) {
  if (msg.type() == PREPARE_REPLY) {
    proposer_.OnPrepareReply(msg);
  } else if (msg.type() == ACCEPT_REPLY) {
    proposer_.OnAccpetReply(msg);
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
  switch(msg.type()) {
    case NEW_CHOSEN_VALUE:
      learner_.OnNewChosenValue(msg);
      break;
    case LEARNER_ASK_FOR_LEARN:
      learner_.OnAskForLearn(msg);
      break;
    case LEARNER_SEND_LEARN_VALUE:
      break;
    case LEARNER_SEND_NOW_INSTANCE_ID:
      learner_.OnSendNowInstanceId(msg);
      break;
    case LEARNER_COMFIRM_ASK_FOR_LEARN:
      learner_.OnComfirmForLearn(msg);
      break;
    default:
      break;
  }
  if (new_instance_) {
    NewInstance();
  }
}

void Instance::NewInstance() {
}

}  // namespace paxos
}  // namespace voyager
