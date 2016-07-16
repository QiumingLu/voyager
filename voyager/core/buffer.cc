#include "voyager/core/buffer.h"

#include <sys/uio.h>
#include <errno.h>

#include "voyager/core/socket_util.h"

namespace voyager {

const char Buffer::kCRLF[] = "\r\n";

Buffer::Buffer(size_t init_size) 
    : buf_(init_size),
      read_index_(0),
      write_index_(0) {
}

ssize_t Buffer::ReadV(int socketfd, int& err) {
  char backup_buf[kBackupBufferSize];
  struct iovec iov[2];
  const size_t writable_size = WritableSize();
  iov[0].iov_base = &*(buf_.begin() + SafeCast(write_index_));
  iov[0].iov_len = writable_size;
  iov[1].iov_base = backup_buf;
  iov[1].iov_len = sizeof(backup_buf);
  int count = (writable_size < sizeof(backup_buf)) ? 2 : 1;
  const ssize_t n = sockets::ReadV(socketfd, iov, count);
  if (n < 0) {
    err = errno;
  } else if (static_cast<size_t>(n) <= writable_size) {
    write_index_ += static_cast<const size_t>(n);
  } else {
    write_index_ = buf_.size();
    Append(backup_buf, static_cast<const size_t>(n) - writable_size);
  }
  return n;
}

}  // namespace voyager
