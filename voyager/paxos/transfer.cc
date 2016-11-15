#include "voyager/paxos/transfer.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace paxos {

Transfer::Transfer(Config* config, IOLoop* loop)
    : config_(config), loop_(loop), mutex_(), station_(loop) {
}

Status Transfer::NewValue(const Slice& value, uint64_t* new_instance_id) {
  port::MutexLock lock(&mutex_);
  station_.TransferToLoop(value);
  station_.GetResult(new_instance_id);
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
