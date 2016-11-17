#include "voyager/paxos/storage/multi_db.h"

#include <assert.h>
#include <unistd.h>

namespace voyager {
namespace paxos {

MultiDB::MultiDB() {
}

MultiDB::~MultiDB() {
  for (size_t i = 0; i < multi_db_.size(); ++i) {
    delete multi_db_[i];
  }
}

void MultiDB::OpenAll(const std::string& path, size_t group_size) {
  if (::access(path.c_str(), F_OK) == -1) {
  }

  std::string temp = path;
  if (path[path.size() - 1] != '/') {
    temp += '/';
  }

  for (size_t i = 0; i < group_size; ++i) {
    char name[512];
    snprintf(name, sizeof(name), "%sg%zu", temp.c_str(), i);
    DB* db = new DB(i, name);
    multi_db_.push_back(db);
  }
}

DB* MultiDB::GetDB(size_t group_idx) const {
  assert(group_idx < multi_db_.size());
  return multi_db_[group_idx];
}

}  // namespace paxos
}  // namespace voyager
