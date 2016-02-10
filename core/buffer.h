#ifndef MIRANTS_CORE_BUFFER_H_
#define MIRANTS_CORE_BUFFER_H_

#include <algorithm>
#include <vector>
#include <assert.h>
#include <stddef.h>
#include <sys/types.h>

namespace mirants {

class Buffer {
 public:
  Buffer(size_t init_size = kInitBufferSize);
  
  ssize_t ReadV(int socketfd, int& err);

//  char* BeginRead() { return Begin() + read_index_; }
//  const char* BeginRead() const { return Begin() + read_index_; }
//  char* BeginWrite() { return Begin() + write_index_; }
//  const char* BeginWrite() const { return Begin() + write_index_; }

  size_t ReadableSize() const { return write_index_ - read_index_; }
  size_t WritableSize() const { return buf_.size() - write_index_; }

  void Append(const char* data, size_t size) {
    if (WritableSize() < size) {
      MakeSpace(size);
    }
    std::copy(data, data+size, buf_.begin()+write_index_);
    write_index_ += size;
  }

 private:
//  char* Begin() { return &*buf_.begin(); }
//  const char* Begin() const { return &*buf_.begin(); }

  void MakeSpace(size_t size) {
    if (WritableSize() < size) {
      buf_.resize(write_index_ + size);
    } else {
      size_t readable_size = ReadableSize();
      std::copy(buf_.begin()+read_index_,
                buf_.begin()+write_index_,
                buf_.begin());
      read_index_ = 0;
      write_index_ = read_index_ + readable_size;
      assert(readable_size == ReadableSize());
    }
  }

  static const size_t kInitBufferSize = 10;
  static const size_t kBackupBufferSize = 102;
  std::vector<char> buf_;
  size_t read_index_;
  size_t write_index_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_BUFFER_H_
