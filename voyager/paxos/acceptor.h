#ifndef VOYAGER_PAXOS_ACCEPTOR_H_
#define VOYAGER_PAXOS_ACCEPTOR_H_

namespace voyager {
namespace paxos {

class Acceptor {
 public:
  Acceptor();

 private:

  // No copying allowed
  Acceptor(const Acceptor&);
  void operator=(const Acceptor&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_ACCEPTOR_H_
