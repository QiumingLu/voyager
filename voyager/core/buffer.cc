// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/buffer.h"

#include <errno.h>
#include <sys/uio.h>

namespace voyager {

const char Buffer::kCRLF[] = "\r\n";

Buffer::Buffer(size_t init_size)
    : buf_(init_size), read_index_(0), write_index_(0) {}

Buffer::Buffer(const Buffer& other) {
  buf_ = other.buf_;
  read_index_ = other.read_index_;
  write_index_ = other.write_index_;
}

Buffer::Buffer(Buffer&& other) {
  buf_ = std::move(other.buf_);
  read_index_ = other.read_index_;
  write_index_ = other.write_index_;
}

Buffer& Buffer::operator=(const Buffer& other) {
  if (this != &other) {
    buf_ = other.buf_;
    read_index_ = other.read_index_;
    write_index_ = other.write_index_;
  }
  return *this;
}

Buffer& Buffer::operator=(Buffer&& other) {
  if (this != &other) {
    buf_ = std::move(other.buf_);
    read_index_ = other.read_index_;
    write_index_ = other.write_index_;
  }
  return *this;
}

ssize_t Buffer::ReadV(int socketfd) {
  char backup_buf[kBackupBufferSize];
  struct iovec iov[2];
  const size_t writable_size = WritableSize();
  iov[0].iov_base = PeekAt(write_index_);
  iov[0].iov_len = writable_size;
  iov[1].iov_base = backup_buf;
  iov[1].iov_len = sizeof(backup_buf);
  int count = (writable_size < sizeof(backup_buf)) ? 2 : 1;
  const ssize_t n = ::readv(socketfd, iov, count);
  if (n != -1) {
    if (static_cast<size_t>(n) <= writable_size) {
      write_index_ += static_cast<const size_t>(n);
    } else {
      write_index_ = buf_.size();
      Append(backup_buf, static_cast<const size_t>(n) - writable_size);
    }
  }
  return n;
}

}  // namespace voyager
