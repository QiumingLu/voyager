#ifndef VOYAGER_PAXOS_IOLOOP_H_
#define VOYAGER_PAXOS_IOLOOP_H_

namespace voyager {
namespace paxos {

class IOLoop {
 public:
  IOLoop();

 private:

  // No copying allowed
  IOLoop(const IOLoop&);
  void operator=(const IOLoop&);
};

}  // namespace paxos
}  // namespace voyager
