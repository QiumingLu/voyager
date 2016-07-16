#ifndef VOYAGER_UTIL_TIMESTAMP_H_
#define VOYAGER_UTIL_TIMESTAMP_H_

#include <stdint.h>

#include <string>

namespace voyager {
namespace timeops {

static const int64_t kSecondsPerMinute = 60;
static const int64_t kSecondsPerHour = 3600;
static const int64_t kSecondsPerDay = kSecondsPerHour * 24;
static const int64_t kMilliSecondsPerSecond = 1000;
static const int64_t kMicroSecondsPerSecond = 1000 * 1000;
static const int64_t kNonasSecondsPerSecond = 1000 * 1000 * 1000;
  
}  // namespace timeops

class Timestamp {
 public:
  Timestamp() : microseconds_since_epoch_(0) { }
  explicit Timestamp(int64_t microseconds) 
      : microseconds_since_epoch_(microseconds) 
  { }

  static Timestamp Now();
  static Timestamp FromUnixTimestamp(time_t t, int microseconds = 0) {
    return Timestamp(
        static_cast<int64_t>(t) * (timeops::kMicroSecondsPerSecond) + 
        microseconds);
  }

  std::string FormatTimestamp() const;

  time_t ToUnixTimestamp() const {
    return static_cast<time_t>(
        microseconds_since_epoch_ / (timeops::kMicroSecondsPerSecond));
  }

  void SetMicroSecondsSinceEpoch(int64_t microsecons) {
    microseconds_since_epoch_ = microsecons;
  }

  int64_t MicroSecondsSinceEpoch() const {
    return microseconds_since_epoch_;
  } 

  bool Valid() const {
    return microseconds_since_epoch_ > 0;
  }

 private:
  int64_t microseconds_since_epoch_;
};

inline Timestamp AddTime(Timestamp timestamp, double seconds) {
  int64_t microseconds = 
      static_cast<int64_t>(seconds * timeops::kMicroSecondsPerSecond);
  return Timestamp(timestamp.MicroSecondsSinceEpoch() + microseconds);
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
  return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
}

inline bool operator<(Timestamp lhs, Timestamp rhs) {
  return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
}

inline bool operator>(Timestamp lhs, Timestamp rhs) {
  return lhs.MicroSecondsSinceEpoch() > rhs.MicroSecondsSinceEpoch();
}

}  // namespace voyager

#endif  // VOYAGER_UTIL_TIMESTAMP_H_
