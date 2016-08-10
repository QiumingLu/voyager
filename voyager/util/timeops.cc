#include "voyager/util/timeops.h"

#include <sys/time.h>
#include <time.h>

#include "voyager/util/stringprintf.h"

namespace voyager {
namespace timeops {

uint64_t NowMicros() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return static_cast<uint64_t>(tv.tv_sec) * kMicroSecondsPerSecond + tv.tv_usec;
}

std::string FormatTimestamp(uint64_t micros) {  
  const time_t seconds = static_cast<time_t>(
      micros / (timeops::kMicroSecondsPerSecond));
  micros = static_cast<int>(
      micros % (timeops::kMicroSecondsPerSecond));

  struct tm t;
  localtime_r(&seconds, &t);
  
  return StringPrintf("%04d/%02d/%02d-%02d:%02d:%02d.%06d",
                      t.tm_year + 1900,
                      t.tm_mon + 1,
                      t.tm_mday,
                      t.tm_hour,
                      t.tm_min,
                      t.tm_sec,
                      micros);
}

}  // namespace timeops
}  // namespace voyager