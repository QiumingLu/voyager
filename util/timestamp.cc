#include "util/timestamp.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

namespace mirants {

Timestamp Timestamp::Now() {
  struct timeval now_tv;
  gettimeofday(&now_tv, NULL);
  int64_t seconds = now_tv.tv_sec;
  return Timestamp(
      seconds * (timedetail::kMicroSecondsPerSecond) + now_tv.tv_usec);
}

std::string Timestamp::FormatTimestamp() const {
  char buffer[32];
  memset(buffer, 0, sizeof(buffer));
  
  const time_t seconds = static_cast<time_t>(
      microseconds_since_epoch_ / (timedetail::kMicroSecondsPerSecond));
  int micro = static_cast<int>(
      microseconds_since_epoch_ % (timedetail::kMicroSecondsPerSecond));

  struct tm t;
  localtime_r(&seconds, &t);
  //gmtime_r(&seconds, &t);

  snprintf(buffer, sizeof(buffer),
                    "%04d/%02d/%02d-%02d:%02d:%02d.%06d",
                    t.tm_year + 1900,
                    t.tm_mon + 1,
                    t.tm_mday,
                    t.tm_hour,
                    t.tm_min,
                    t.tm_sec,
                    micro);
  return buffer;
}

}  // namespace mirants
