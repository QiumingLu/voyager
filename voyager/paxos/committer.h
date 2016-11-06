#ifndef VOYAGER_PAXOS_COMMITTER_H_
#define VOYAGER_PAXOS_COMMITTER_H_

namespace voyager {
namespace paxos {

class Committer {
 public:
  Committer();

 private:
  // No copying allowed
  Committer(const Committer&);
  void operator=(const Committer&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_COMMITTER_H_
