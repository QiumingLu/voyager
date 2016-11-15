#ifndef VOYAGER_PAXOS_IOLOOP_H_
#define VOYAGER_PAXOS_IOLOOP_H_

#include <deque>

#include "voyager/port/thread.h"
#include "voyager/port/mutex.h"
#include "voyager/util/slice.h"

namespace voyager {
namespace paxos {

class Instance;

class IOLoop {
 public:
  IOLoop(Instance* instance, const std::string& name = std::string());
  ~IOLoop();

  void Loop();
  void Exit();

  void NewValue(const Slice& value);
  void NewMessage(const Slice& s);

 private:
  void ThreadFunc();

  bool exit_;
  Instance* instance_;
  port::Thread thread_;

  port::Mutex mutex_;
  port::Condition cond_;
  Slice value_;
  std::deque<std::string*> messages_;

  // No copying allowed
  IOLoop(const IOLoop&);
  void operator=(const IOLoop&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_IOLOOP_H_
