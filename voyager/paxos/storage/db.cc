#include "voyager/paxos/storage/db.h"

#include <leveldb/options.h>
#include <leveldb/status.h>

namespace voyager {
namespace paxos {

DB::DB(size_t group_idx, const std::string& name)
    : group_idx_(group_idx), name_(name) {
  leveldb::Options options;
  options.create_if_missing = true;
  options.write_buffer_size = 1024 * 1024 + group_idx * 10 * 1024;
  leveldb::Status status = leveldb::DB::Open(options, name_, &db_);
  if (!status.ok()) {
  }
}

DB::~DB() {
  delete db_;
}

int DB::Put(const WriteOptions& options,
            uint64_t instance_id,
            const std::string& value) {
  char key[8];
  memcpy(key, &instance_id, sizeof(instance_id));
  leveldb::WriteOptions op;
  op.sync = options.sync;
  leveldb::Status status = db_->Put(op, key, value);
  if (!status.ok()) {
    return -1;
  }
  return 0;
}

int DB::Delete(const WriteOptions& options, uint64_t instance_id) {
  char key[8];
  memcpy(key, &instance_id, sizeof(instance_id));
  leveldb::WriteOptions op;
  op.sync = options.sync;
  leveldb::Status status = db_->Delete(op, key);
  if (!status.ok()) {
    return -1;
  }
  return 0;
}

int DB::Get(uint64_t instance_id, std::string* value) {
  char key[8];
  memcpy(key, &instance_id, sizeof(instance_id));
  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, value);
  if (!status.ok()) {
    return -1;
  }
  return 0;
}

int DB::GetMaxInstanceId(uint64_t* instance_id) {
  leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());
  it->SeekToLast();
  while (it->Valid()) {
    memcpy(instance_id, it->key().data(), sizeof(uint64_t));
    delete it;
    return 0;
  }
  delete it;
  return 1;
}

}  // namespace paxos
}  // namespace voyager
