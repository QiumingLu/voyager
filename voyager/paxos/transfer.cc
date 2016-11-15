#include "voyager/paxos/transfer.h"

namespace voyager {
namespace paxos {

Transfer::Transfer(Config* config, IOLoop* loop)
    : config_(config), loop_(Loop), mutex_() {
}

Status Transfer::NewValue(const Slice& value, uint64_t* new_instance_id) {
  port::MutexLock lock(&mutex_);
  return Status::OK();
}
