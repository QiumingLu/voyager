#ifndef VOYAGER_PAXOS_STORAGE_DB_H_
#define VOYAGER_PAXOS_STORAGE_DB_H_

#include <stdint.h>
#include <string>
#include <leveldb/db.h>

namespace voyager {
namespace paxos {

struct WriteOptions {
  bool sync;

  WriteOptions()
      : sync(true) {
  }
};

class DB {
 public:
  DB(size_t group_idx, const std::string& name);
  ~DB();

  int Open(const std::string& name, size_t group_idx);

  int Put(const WriteOptions& options,
          uint64_t instance_id,
          const std::string& value);

  int Delete(const WriteOptions& options, uint64_t instance_id);

  int Get(uint64_t instance_id, std::string* value);

  int GetMaxInstanceId(uint64_t* instance_id);

 private:
  size_t group_idx_;
  leveldb::DB* db_;
  std::string name_;

  // No copying allowed
  DB(const DB&);
  void operator=(const DB&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_STORAGE_DB_H_
