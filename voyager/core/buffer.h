// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_BUFFER_H_
#define VOYAGER_CORE_BUFFER_H_

#include <assert.h>
#include <stddef.h>
#include <sys/types.h>

#include <algorithm>
#include <string>
#include <vector>

#include "voyager/util/slice.h"

namespace voyager {

class Buffer {
 public:
  explicit Buffer(size_t init_size = kInitBufferSize);

  ssize_t ReadV(int socketfd);

  size_t ReadableSize() const { return write_index_ - read_index_; }
  size_t WritableSize() const { return buf_.size() - write_index_; }

  void Append(const Slice& s) {
    Append(s.data(), s.size());
  }

  void Append(const char* data, size_t size) {
    if (WritableSize() < size) {
      MakeSpace(size);
    }
    std::copy(
        data,
        data+size,
        buf_.begin() + static_cast<std::ptrdiff_t>(write_index_));
    write_index_ += size;
  }

  const char* Peek() const {
    return PeekAt(read_index_);
  }

  const char* FindCRLF() {
    const char* crlf = std::search(PeekAt(read_index_), PeekAt(write_index_),
                                   kCRLF, kCRLF + 2);
    if (crlf == PeekAt(write_index_)) {
      return nullptr;
    } else {
      return crlf;
    }
  }

  void Retrieve(size_t size) {
    assert(size <= ReadableSize());
    if (size < ReadableSize()) {
      read_index_ += size;
    } else {
      RetrieveAll();
    }
  }

  void RetrieveUntil(const char* end) {
    assert(Peek() <= end);
    assert(end <= PeekAt(write_index_));
    Retrieve(static_cast<size_t>(end - Peek()));
  }

  void RetrieveAll() {
    read_index_ = write_index_ = 0;
  }

  std::string RetrieveAllAsString() {
    return RetrieveAsString(ReadableSize());
  }

  std::string RetrieveAsString(size_t size) {
    assert(size <= ReadableSize());
    std::string result(Peek(), size);
    Retrieve(size);
    return result;
  }

 private:
  inline char* PeekAt(size_t index) {
    if (buf_.empty()) {
      return nullptr;
    }
    return &*(buf_.begin() + static_cast<std::ptrdiff_t>(index));
  }

  inline const char* PeekAt(size_t index) const {
    if (buf_.empty()) {
      return nullptr;
    }
    return &*(buf_.begin() + static_cast<std::ptrdiff_t>(index));
  }

  void MakeSpace(size_t size) {
    if (WritableSize() < size) {
      buf_.resize(write_index_ + size);
    } else {
      size_t readable_size = ReadableSize();
      std::copy(buf_.begin() + static_cast<std::ptrdiff_t>(read_index_),
                buf_.begin() + static_cast<std::ptrdiff_t>(write_index_),
                buf_.begin());
      read_index_ = 0;
      write_index_ = read_index_ + readable_size;
      assert(readable_size == ReadableSize());
    }
  }

  static const size_t kInitBufferSize = 1024;
  static const size_t kBackupBufferSize = 65536;
  static const char kCRLF[];

  std::vector<char> buf_;
  size_t read_index_;
  size_t write_index_;
};

}  // namespace voyager

#endif  // VOYAGER_CORE_BUFFER_H_
