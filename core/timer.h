#ifndef MIRANTS_CORE_TIMER_H_
#define MIRANTS_CORE_TIMER_H_

#include "core/callback.h"
#include "util/timestamp.h"

namespace mirants {

class Timer {
 public:
  Timer(const TimeProcCallback& func, Timestamp time, double interval) { }

 private:
  const TimeProcCallback timeproc_;
  Timestamp expiration_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TIMER_H_
