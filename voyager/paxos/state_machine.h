#ifndef VOYAGER_PAXOS_STATE_MACHINE_H_
#define VOYAGER_PAXOS_STATE_MACHINE_H_

#include <stddef.h>
#include <stdint.h>
#include <string>

namespace voyager {
namespace paxos {

struct MachineContext {
  int machine_id;
  void* context;
};

class StateMachine {
 public:
  StateMachine();
  virtual ~StateMachine();

  virtual bool Execute(size_t group_idx, uint64_t instance_id,
                       const std::string& value, MachineContext* context) = 0;

  virtual int GetMachineId() const = 0;

 private:
  // No copying allowed
  StateMachine(const StateMachine&);
  void operator=(const StateMachine&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_STATE_MACHINE_H_
