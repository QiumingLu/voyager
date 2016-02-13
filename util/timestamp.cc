#include "util/timestamp.h"

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>

#include "util/stringprintf.h"

namespace mirants {

Timestamp Timestamp::Now() {
  struct timeval now_tv;
  gettimeofday(&now_tv, NULL);
  int64_t seconds = now_tv.tv_sec;
  return Timestamp(
      seconds * (timeops::kMicroSecondsPerSecond) + now_tv.tv_usec);
}

std::string Timestamp::FormatTimestamp() const {  
  const time_t seconds = static_cast<time_t>(
      microseconds_since_epoch_ / (timeops::kMicroSecondsPerSecond));
  int micro = static_cast<int>(
      microseconds_since_epoch_ % (timeops::kMicroSecondsPerSecond));

  struct tm t;
  localtime_r(&seconds, &t);
  //gmtime_r(&seconds, &t);
  
   return StringPrintf("%04d/%02d/%02d-%02d:%02d:%02d.%06d",
                       t.tm_year + 1900,
                       t.tm_mon + 1,
                       t.tm_mday,
                       t.tm_hour,
                       t.tm_min,
                       t.tm_sec,
                       micro);
}

}  // namespace mirants
