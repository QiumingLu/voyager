#ifndef MIRANTS_UTIL_LOGGING_H_
#define MIRANTS_UTIL_LOGGING_H_

#include <string>

namespace mirants {

enum LogLevel {
  LOGLEVEL_TRACE,
  LOGLEVEL_DEBUG,
  LOGLEVEL_INFO,
  LOGLEVEL_WARN,
  LOGLEVEL_ERROR,
  LOGLEVEL_FATAL
};

class Status;
class Slice;
class LogFinisher;

class Logger {
 public:
  Logger(LogLevel level, const char* filename, int line);
  ~Logger() { }

  Logger& operator<<(char value);
  Logger& operator<<(short value);
  Logger& operator<<(unsigned short value);
  Logger& operator<<(int value);
  Logger& operator<<(unsigned int value);
  Logger& operator<<(long value);
  Logger& operator<<(unsigned long value);
  Logger& operator<<(long long value);
  Logger& operator<<(unsigned long long value);
  Logger& operator<<(double value);
  Logger& operator<<(void* value);
  Logger& operator<<(const char* value);
  Logger& operator<<(const Slice& value);
  Logger& operator<<(const std::string& value);
  Logger& operator<<(const Status& value);

 private:
  friend class LogFinisher;
  void Finish();

  LogLevel level_;
  const char* filename_;
  int line_;
  std::string message_;
};

class LogFinisher {
 public:
  void operator=(Logger& logger);
};

#define MIRANTS_LOG(LEVEL)                                \
  LogFinisher() =                                         \
    ::mirants::Logger(                                    \
      ::mirants::LOGLEVEL_##LEVEL, __FILE__, __LINE__)

template<typename T>
T* CheckNotNull(const char* /* filename */, int /* line */, 
                const char* logmessage, T* ptr) {
  if (ptr == NULL) {
    MIRANTS_LOG(FATAL) << logmessage;
  }
}

#define CHECK_NOTNULL(value)  \
  ::mirants::CheckNotNull(__FILE__, __LINE__, \
      "'" #value "' Must not be NULL", (value))


typedef void LogHandler(LogLevel level, const char* filename, 
                        int line, const std::string& message_);
LogHandler* SetLogHandler(LogHandler* new_func);


}  // namespace mirants

#endif  // MIRANTS_UTIL_LOGGING_H_
