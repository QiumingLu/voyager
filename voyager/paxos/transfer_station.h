#ifndef VOYAGER_PAXOS_TRANSFER_STATION_H_
#define VOYAGER_PAXOS_TRANSFER_STATION_H_

#include <stdint.h>

#include "voyager/paxos/ioloop.h"
#include "voyager/port/mutex.h"
#include "voyager/util/slice.h"

namespace voyager {
namespace paxos {

class TransferStation {
 public:
  TransferStation(IOLoop* loop);

  void TransferToLoop(const Slice& value);

  void SetResult(uint64_t instance_id_);
  void GetResult(uint64_t* new_instance_id);

 private:
  IOLoop* loop_;

  port::Mutex mutex_;
  port::Condition cond_;
  uint64_t instance_id_;
  bool transfer_end_;

  // No copying allowed
  TransferStation(const TransferStation&);
  void operator=(const TransferStation&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_TRANSFER_STATION_H_
