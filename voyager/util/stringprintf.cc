// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/stringprintf.h"

namespace voyager {

void StringAppendV(std::string* dst, const char* format, va_list ap) {
  static const int kSpaceLength = 128;
  char space[kSpaceLength];

  va_list backup_ap;
  va_copy(backup_ap, ap);
  int result = vsnprintf(space, kSpaceLength, format, backup_ap);
  va_end(backup_ap);

  if (result < kSpaceLength) {
    if (result >= 0) {
      dst->append(space, static_cast<size_t>(result));
      return;
    }
    if (result < 0) {
      // Error
      return;
    }
  }

  int length = result + 1;
  char* buf = new char[length];

  va_copy(backup_ap, ap);
  result = vsnprintf(buf, static_cast<size_t>(length), format, backup_ap);
  va_end(backup_ap);

  if (result >= 0 && result < length) {
    dst->append(buf, static_cast<size_t>(result));
  }
  delete[] buf;
}

std::string StringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  std::string result;
  StringAppendV(&result, format, ap);
  va_end(ap);
  return result;
}

const std::string& SStringPrintf(std::string* dst, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  dst->clear();
  StringAppendV(dst, format, ap);
  va_end(ap);
  return *dst;
}

void StringAppendF(std::string* dst, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  StringAppendV(dst, format, ap);
  va_end(ap);
}

}  // namespace voyager
