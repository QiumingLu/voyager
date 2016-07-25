#ifndef VOYAGER_CORE_SCHEDULE_H_
#define VOYAGER_CORE_SCHEDULE_H_

#include <string>
#include <vector>

#include "voyager/core/bg_eventloop.h"
#include "voyager/util/scoped_ptr.h"

namespace voyager {

class EventLoop;

class Schedule {
 public:
  Schedule(EventLoop* evloop, int size);

  void Start();

  EventLoop* AssignLoop();

  bool Started() const { return started_; }
 
 private:
  EventLoop* baseloop_;
  size_t size_;
  size_t next_;
  bool started_;
  
  scoped_array<scoped_ptr<BGEventLoop> > loops_;
  std::vector<EventLoop*> ptrs_;

  // No copying allow
  Schedule(const Schedule&);
  void operator=(const Schedule&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_SCHEDULE_H_
