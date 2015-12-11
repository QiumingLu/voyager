#ifndef MIRANTS_UTIL_STATUS_H_
#define MIRANTS_UTIL_STATUS_H_

#include <string>
#include "util/slice.h"

namespace mirants {

class Status {
 public:
  Status() : state_(NULL) { } 
  ~Status() { delete[] state_; }

  // Copy the specified status.
  Status(const Status& s);
  Status(Status&& s);
  void operator=(const Status& s);
  void operator=(Status&& s);

  static Status OK() { return Status(); }
  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotFound, msg, msg2);
  }
  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kCorruption, msg, msg2);
  }
  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotSupported, msg, msg2);
  }
  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kInvalidArgument, msg, msg2);
  }
  static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kIOError, msg, msg2);
  }
  
  bool ok() const { return state_ == NULL; }
  bool IsNotFound() const { return code() == kNotFound; }
  bool IsCorruption() const { return code() == kCorruption; }
  bool IsNotSupported() const { return code() == kNotSupported; }
  bool IsInvalidArgument() const { return code() == kInvalidArgument; }
  bool IsIOError() const { return code() == kIOError; }

  std::string ToString() const;

 private:
  // OK status has a NULL state_. Otherwise, state_ is a new[] array
  // of the following form:
  //     state_[0...3] = length of message
  //     state_[4]     = code
  //     state_[5...]  = message
  const char* state_;
  enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5
  };

  Code code() const {
    return (state_ == NULL) ? kOk : static_cast<Code>(state_[4]);
  }

  Status(Code code, const Slice& msg, const Slice& msg2);
  static const char* CopyState(const char* s);
};

inline Status::Status(const Status& s) {
  state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
}

inline Status::Status(Status&& s) {
  state_ = (s.state_ == NULL) ? NULL : s.state_;
  s.state_ = NULL;
}

inline void Status::operator=(const Status& s) {
  if (state_ != s.state_) {
    delete[] state_;
    state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
  }
}

inline void Status::operator=(Status&& s) {
  if (state_ != s.state_) {
    delete[] state_;
    state_ = s.state_;
    s.state_ = NULL;
  }
}

}  // namespace mirants

#endif  // MIRANTS_UTIL_STATUS_H_
