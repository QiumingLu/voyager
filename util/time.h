#ifndef MIRANTS_UTIL_TIME_H_
#define MIRANTS_UTIL_TIME_H_

namespace mirants {
namespace timedetail {
  
static const int64_t kSecondsPerMinute = 60;
static const int64_t kSecondsPerHour = 3600;
static const int64_t kSecondsPerDay = kSecondsPerHour * 24;
static const int64_t kMilliSecondsPerSecond = 1000;
static const int64_t kMicroSecondsPerSecond = 1000 * 1000;
static const int64_t kNonasSecondsPerSecond = 1000 * 1000 * 1000;

}  // namespace timedetail
}  // namespace mirants

#endif  // MIRANTS_UTIL_TIME_H_
