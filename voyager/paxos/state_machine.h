#ifndef VOYAGER_PAXOS_STATE_MACHINE_H_
#define VOYAGER_PAXOS_STATE_MACHINE_H_

namespace voyager {
namespace paxos {

class StateMachine {
 public:
  StateMachine();

 private:
  // No copying allowed
  StateMachine(const StateMachine&);
  void StateMachine(const StateMachine&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_STATE_MACHINE_H_
