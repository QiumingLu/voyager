#ifndef VOYAGER_PAXOS_COMMITTER_H_
#define VOYAGER_PAXOS_COMMITTER_H_

#include <string>

#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Committer {
 public:
  Committer();

  Status NewValue(const std::string& value);

 private:
  // No copying allowed
  Committer(const Committer&);
  void operator=(const Committer&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_COMMITTER_H_
