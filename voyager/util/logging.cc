// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <functional>
#include <sstream>
#include <utility>

#include "voyager/util/slice.h"
#include "voyager/util/status.h"

namespace voyager {

void DefaultLogHandler(LogLevel level, const char* filename, int line,
                       const std::string& message) {
  static const char* kLogLevelNames[] = {"DEBUG", "INFO ", "WARN ", "ERROR",
                                         "FATAL"};

  char log_time[64];
  struct timeval now_tv;
  gettimeofday(&now_tv, nullptr);
  const time_t seconds = now_tv.tv_sec;
  struct tm t;
  localtime_r(&seconds, &t);
  snprintf(log_time, sizeof(log_time), "%04d/%02d/%02d-%02d:%02d:%02d.%06d",
           t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min,
           t.tm_sec, static_cast<int>(now_tv.tv_usec));

  fprintf(stderr, "[%s][%s %s:%d] %s\n", log_time, kLogLevelNames[level],
          filename, line, message.c_str());
}

void NullLogHandler(LogLevel /* level */, const char* /* filename */,
                    int /* line */, const std::string& /* message */) {}

static LogHandler* log_handler_ = &DefaultLogHandler;
static LogLevel log_level_ = LOGLEVEL_WARN;

Logger::Logger(LogLevel level, const char* filename, int line)
    : level_(level), filename_(filename), line_(line) {}

Logger& Logger::operator<<(const char* value) {
  message_ += value;
  return *this;
}

Logger& Logger::operator<<(const Slice& value) {
  message_.append(value.data(), value.size());
  return *this;
}

Logger& Logger::operator<<(const std::string& value) {
  message_ += value;
  return *this;
}

Logger& Logger::operator<<(std::string&& value) {
  if (message_.empty()) {
    message_ = std::move(value);
  } else {
    message_ += value;
  }
  return *this;
}

Logger& Logger::operator<<(const Status& value) {
  message_ += value.ToString();
  return *this;
}

Logger& Logger::operator<<(const std::thread::id& value) {
  std::stringstream ss;
  ss << value;
  message_ += ss.str();
  return *this;
}

#undef DECLARE_STREAM_OPERATOR
#define DECLARE_STREAM_OPERATOR(TYPE, FORMAT)        \
  Logger& Logger::operator<<(TYPE value) {           \
    char buffer[128];                                \
    snprintf(buffer, sizeof(buffer), FORMAT, value); \
    buffer[sizeof(buffer) - 1] = '\0';               \
    message_ += buffer;                              \
    return *this;                                    \
  }

DECLARE_STREAM_OPERATOR(char, "%c")
DECLARE_STREAM_OPERATOR(short, "%d")
DECLARE_STREAM_OPERATOR(unsigned short, "%u")
DECLARE_STREAM_OPERATOR(int, "%d")
DECLARE_STREAM_OPERATOR(unsigned int, "%u")
DECLARE_STREAM_OPERATOR(long, "%ld")
DECLARE_STREAM_OPERATOR(unsigned long, "%lu")
DECLARE_STREAM_OPERATOR(long long, "%lld")
DECLARE_STREAM_OPERATOR(unsigned long long, "%llu")
DECLARE_STREAM_OPERATOR(double, "%g")
DECLARE_STREAM_OPERATOR(void*, "%p")
DECLARE_STREAM_OPERATOR(const void*, "%p")
#undef DECLARE_STREAM_OPERATOR

void Logger::Finish() {
  if (level_ >= log_level_) {
    log_handler_(level_, filename_, line_, message_);
  }

  if (level_ == LOGLEVEL_FATAL) {
#if USE_EXCEPTIONS
    throw FatalException(filename_, line_, message_);
#else
    abort();
#endif
  }
}

void LogFinisher::operator=(Logger& logger) { logger.Finish(); }

LogHandler* SetLogHandler(LogHandler* new_func) {
  LogHandler* old = log_handler_;
  if (old == &NullLogHandler) {
    old = nullptr;
  }
  if (new_func == nullptr) {
    log_handler_ = &NullLogHandler;
  } else {
    log_handler_ = new_func;
  }
  return old;
}

LogLevel SetLogLevel(LogLevel new_level) {
  LogLevel old_level = log_level_;
  log_level_ = new_level;
  return old_level;
}

}  // namespace voyager
