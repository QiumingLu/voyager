#include "voyager/paxos/storage/db.h"

#include <leveldb/options.h>
#include <leveldb/status.h>

namespace voyager {
namespace paxos {

DB::DB()
    : has_open_(false), group_idx_(0), db_(nullptr), name_() {
}

DB::~DB() {
  delete db_;
}

int DB::Open(size_t group_idx, const std::string& name) {
  if (has_open_) {
    return 0;
  }
  group_idx_ = group_idx;
  name_ = name;
  leveldb::Options options;
  options.create_if_missing = true;
  options.write_buffer_size = 1024 * 1024 + group_idx * 10 * 1024;
  leveldb::Status status = leveldb::DB::Open(options, name, &db_);

  if (!status.ok()) {
    return -1;
  }

  has_open_ = true;
  return 0;
}

int DB::Put(const WriteOptions& options,
            uint64_t instance_id,
            const std::string& value) {
  if (!has_open_) {
    return -1;
  }

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
  if (!has_open_) {
    return -1;
  }

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
  if (!has_open_) {
    return -1;
  }
  char key[8];
  memcpy(key, &instance_id, sizeof(instance_id));
  leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, value);
  if (!status.ok()) {
    return -1;
  }
  return 0;
}

}  // namespace paxos
}  // namespace voyager
