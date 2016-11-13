#ifndef VOYAGER_PAXOS_IOLOOP_H_
#define VOYAGER_PAXOS_IOLOOP_H_

#include <deque>

#include "voyager/port/thread.h"
#include "voyager/port/mutex.h"

namespace voyager {
namespace paxos {

class Instance;

class IOLoop {
 public:
  IOLoop(Instance* instance, const std::string& name = std::string());

  void Loop();
  void Exit();

  void NewMessage(const char* s, size_t n);

 private:
  void ThreadFunc();

  Instance* instance_;
  port::Thread thread_;

  port::Mutex mutex_;
  std::deque<std::string*> messages_;

  // No copying allowed
  IOLoop(const IOLoop&);
  void operator=(const IOLoop&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_IOLOOP_H_
