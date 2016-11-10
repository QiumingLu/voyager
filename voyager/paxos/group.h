#ifndef VOYAGER_PAXOS_GROUP_H_
#define VOYAGER_PAXOS_GROUP_H_

#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

class Config;

class Group {
 public:
  Group(const Config* config);

  Instance* GetInstance() { return &instance_; }

 private:
  Instance instance_;

  // No copying allowed
  Group(const Group&);
  void operator=(const Group&);
};

}  // namespace paxos
}  // namespace voyager
#endif  // VOYAGER_PAXOS_GROUP_H_
