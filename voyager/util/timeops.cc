// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/timeops.h"

#include <sys/time.h>
#include <time.h>

namespace voyager {
namespace timeops {

uint64_t NowMicros() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

std::string FormatTimestamp(uint64_t micros) {
  const time_t seconds = static_cast<time_t>(micros / 1000000);
  micros = static_cast<int>(micros % 1000000);

  struct tm t;
  localtime_r(&seconds, &t);

  char buf[128];
  snprintf(buf, sizeof(buf), "%04d/%02d/%02d-%02d:%02d:%02d.%06d",
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
           t.tm_sec, static_cast<int>(micros));

  return std::string(buf);
}

}  // namespace timeops
}  // namespace voyager
