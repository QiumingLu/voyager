#ifndef MIRANTS_UTIL_TIMESTAMP_H_
#define MIRANTS_UTIL_TIMESTAMP_H_

#include <string>
#include <stdint.h>
#include "util/time.h"

namespace mirants {

class Timestamp {
 public:
  Timestamp() : microseconds_since_epoch_(0) { }
  explicit Timestamp(int64_t microseconds) 
      : microseconds_since_epoch_(microseconds) 
  { }

  static Timestamp Now();
  static Timestamp FromUnixTimestamp(time_t t, int microseconds = 0) {
    return Timestamp(
        static_cast<int64_t>(t) * (timedetail::kMicroSecondsPerSecond) + 
        microseconds);
  }

  std::string FormatTimestamp() const;

  time_t ToUnixTimestamp() const {
    return static_cast<time_t>(
        microseconds_since_epoch_ / (timedetail::kMicroSecondsPerSecond));
  }

  void SetMicroSecondsSinceEpoch(int64_t microsecons) {
    microseconds_since_epoch_ = microsecons;
  }
  int64_t MicroSecondsSinceEpoch() const {
    return microseconds_since_epoch_;
  } 
  
  bool CompareTo(Timestamp& rhs) const {
    return microseconds_since_epoch_ > rhs.microseconds_since_epoch_;
  }

  bool Equal(Timestamp& rhs) const {
    return microseconds_since_epoch_ == rhs.microseconds_since_epoch_;
  }

 private:
  int64_t microseconds_since_epoch_;
};

inline Timestamp AddTime(Timestamp timestamp, int64_t microseconds) {
  return Timestamp(timestamp.MicroSecondsSinceEpoch() + microseconds);
}

inline bool operator==(const Timestamp& x, const Timestamp& y) {
  return x.MicroSecondsSinceEpoch() == y.MicroSecondsSinceEpoch();
}

inline bool operator!=(const Timestamp& x, const Timestamp& y) {
  return x.MicroSecondsSinceEpoch() != y.MicroSecondsSinceEpoch();
}

inline bool operator>(const Timestamp& x, const Timestamp& y) {
  return x.MicroSecondsSinceEpoch() > y.MicroSecondsSinceEpoch();
}

inline bool operator<(const Timestamp& x, const Timestamp& y) {
  return x.MicroSecondsSinceEpoch() < y.MicroSecondsSinceEpoch();
}

inline bool operator>=(const Timestamp& x, const Timestamp& y) {
  return x.MicroSecondsSinceEpoch() >= y.MicroSecondsSinceEpoch();
}

inline bool operator<=(const Timestamp& x, const Timestamp& y) {
  return x.MicroSecondsSinceEpoch() <= y.MicroSecondsSinceEpoch();
}

}  // namespace mirants

#endif  // MIRANTS_UTIL_TIMESTAMP_H_
