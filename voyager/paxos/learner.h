#ifndef VOYAGER_PAXOS_LEARNER_H_
#define VOYAGER_PAXOS_LEARNER_H_

#include <stdint.h>
#include <functional>
#include <memory>
#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/paxos.pb.h"

namespace voyager {
namespace paxos {

class Config;
class Instance;

class Learner {
 public:
  typedef std::function<void(const PaxosMessage& )> NewChosenValueCallback;

  Learner(Config* config, Acceptor* acceptor);

  void SetNewChosenValueCallback(const NewChosenValueCallback& cb) {
    new_chosen_cb_ = cb;
  }
  void SetNewChosenValueCallback(NewChosenValueCallback&& cb) {
    new_chosen_cb_ = std::move(cb);
  }

  void SetInstanceId(uint64_t instance_id) { instance_id_ = instance_id; }

  void NewChosenValue(uint64_t instance_id, uint64_t proposer_id);
  void OnNewChosenValue(const PaxosMessage& msg);

  void OnAskForLearn(const PaxosMessage& msg);
  void OnSendNowInstanceId(const PaxosMessage& msg);
  void OnComfirmForLearn(const PaxosMessage& msg);

 private:
  void AskForLearn();
  void SendNowInstanceId(const PaxosMessage& msg);
  void ComfirmForLearn(const PaxosMessage& msg);

  Config* config_;
  Acceptor* acceptor_;

  uint64_t instance_id_;

  NewChosenValueCallback new_chosen_cb_;

  // No copying allowed
  Learner(const Learner&);
  void operator=(const Learner&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_LEARNER_H_
