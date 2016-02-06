#ifndef MIRANTS_CORE_BUFFER_H_
#define MIRANTS_CORE_BUFFER_H_

#include <vector>
#include <stddef.h>
#include <sys/types.h>

namespace mirants {

class Buffer {
 public:
  Buffer(size_t init_size = kInitBufferSize);
  
  ssize_t ReadV(int socketfd, int& err);

  size_t ReadableSize() const { return write_index_ - read_index_; }
  size_t WritableSize() const { return buf_.size() - write_index_; }

 private:
  char* Begin() { return &*buf_.begin(); }
  const char* Begin() const { return &*buf_.begin(); }

  static const size_t kInitBufferSize = 1024;
  std::vector<char> buf_;
  size_t read_index_;
  size_t write_index_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_BUFFER_H_
