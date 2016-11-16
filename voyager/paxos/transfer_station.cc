#include "voyager/paxos/transfer_station.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace paxos {

TransferStation::TransferStation(RunLoop* loop)
    : loop_(loop), mutex_(), cond_(&mutex_),
      instance_id_(-1), transfer_end_(false) {
}

void TransferStation::TransferToLoop(const Slice& value) {
  instance_id_ = -1;
  transfer_end_ = false;
  loop_->NewValue(value);
}

void TransferStation::SetResult(uint64_t instance_id) {
  port::MutexLock lock(&mutex_);
  instance_id_ = instance_id;
  transfer_end_ = true;
  cond_.Signal();
}

void TransferStation::GetResult(uint64_t* new_instance_id) {
  port::MutexLock lock(&mutex_);
  while (!transfer_end_) {
    cond_.Wait();
  }
  *new_instance_id = instance_id_;
}

}  // namespace paxos
}  // namespace voyager
