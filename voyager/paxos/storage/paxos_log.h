#ifndef VOYAGER_PAXOS_STORAGE_PAXOS_LOG_H_
#define VOYAGER_PAXOS_STORAGE_PAXOS_LOG_H_

namespace voyager {
namespace paxos {

class PaxosLog {
 public:
  PaxosLog();
  ~PaxosLog();

  int WriteLog(const WriteOptions& options, size_t group_idx,
               uint64_t instance_id, const std::string& value);
  int ReadLog(size_t group_idx, uint64_t instance_id, std::string* value);

  int GetMaxInstanceId(size_t group_idx, uint64_t* instance_id);

 private:
  MultiDB db_;

  // No copying allowed
  PaxosLog(const PaxosLog&);
  void operator=(const PaxosLog&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_STORAGE_PAXOS_LOG_H_
