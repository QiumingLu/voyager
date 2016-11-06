#ifndef VOYAGER_PAXOS_GROUP_H_
#define VOYAGER_PAXOS_GROUP_H_

namespace voyager {
namespace paxos {

class Group {
 public:
  Group();

 private:

  // No copying allowed
  Group(const Group&);
  void operator=(const Group&);
};

}  // namespace paxos
}  // namespace voyager
#endif  // VOYAGER_PAXOS_GROUP_H_
