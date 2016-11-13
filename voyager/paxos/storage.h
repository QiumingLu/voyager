#ifndef VOYAGER_PAXOS_STORAGE_H_
#define VOYAGER_PAXOS_STORAGE_H_

#include "voyager/paxos/options.h"

namespace voyager {
namespace paxos {

class LogStorage {
 public:
  LogStorage(const Options* options);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_STORAGE_H_
