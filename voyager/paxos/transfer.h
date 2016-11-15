#ifndef VOYAGER_PAXOS_TRANSFER_H_
#define VOYAGER_PAXOS_TRANSFER_H_

#include "voyager/port/mutex.h"
#include "voyager/util/slice.h"

namespace voyager {
namespace paxos {

class Transfer {
 public:
  Transfer(Config* config, IOLoop* loop);

  Status NewValue(const Slice& value, uint64_t* new_instance_id);

 private:
  Config* config_;
  IOLoop* loop_;
  port::Mutex mutex_;

  // No copying allowed
  Transfer(const Transfer& );
  void operator=(const Transfer&);
};

}  // namespace voyager
}  // namespace paxos
#endif  // VOYAGER_PAXOS_TRANSFER_H_
