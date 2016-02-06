#include "core/buffer.h"

#include <sys/uio.h>
#include "socket_util.h"

namespace mirants {

Buffer::Buffer(size_t init_size) 
    : buf_(init_size),
      read_index_(0),
      write_index_(0) {
}

ssize_t Buffer::ReadV(int socketfd, int& err) {
  char extrabuf[10240];
  struct iovec iov[2];
  const size_t writable_size = WritableSize();
  iov[0].iov_base = Begin() + write_index_;
  iov[0].iov_len = writable_size;
  iov[1].iov_base = extrabuf;
  iov[1].iov_len = sizeof(extrabuf);
  int count = (writable_size < sizeof(extrabuf)) ? 2 : 1;
  const ssize_t n = sockets::ReadV(socketfd, iov, count);
  if (n < 0) {
    err = errno;
  } else if (static_cast<size_t>(n) <= writable_size) {
    write_index_ += n;
  } else {
    write_index_ = buf_.size();
//    Append(extrabuf, n - writable_size);
  }
  return n;
}

}  // namespace mirants
