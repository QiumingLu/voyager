#ifndef VOYAGER_PAXOS_STATE_MACHINE_H_
#define VOYAGER_PAXOS_STATE_MACHINE_H_

namespace voyager {
namespace paxos {

struct SMContext {
  int sm_id;
  void* context;
};

class StateMachine {
 public:
  StateMachine();

  bool Execute(size_t group_idx, uint64_t instance_id,
               const std::string& value, SMContext* context);

 private:
  // No copying allowed
  StateMachine(const StateMachine&);
  void StateMachine(const StateMachine&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_STATE_MACHINE_H_
