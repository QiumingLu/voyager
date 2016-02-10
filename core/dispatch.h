#ifndef MIRANTS_CORE_DISPATCH_H_
#define MIRANTS_CORE_DISPATCH_H_

#include <functional>
#include <memory>
#include <sys/poll.h>

namespace mirants {

class EventLoop;
class Dispatch {
 public:
  typedef std::function<void()> EventCallback;

  Dispatch(EventLoop* eventloop, int fd);
  ~Dispatch();

  int Fd() const { return fd_; }
  int Events() const { return events_; }
  void SetRevents(int rv) { revents_ = rv; }
  void set_index(int index) { index_ = index; }
  int index() const { return index_; }

  void HandleEvent();

  void SetReadCallback(const EventCallback&& func) { readfunc_ = func; }
  void SetReadCallback(EventCallback&& func) { readfunc_ = std::move(func); }
  void SetWriteCallback(const EventCallback& func) { writefunc_ = func; }
  void SetWriteCallback(EventCallback&& func) { writefunc_ = std::move(func); }
  void SetCloseCallback(const EventCallback& func) { closefunc_ = func; }
  void SetCloseCallback(EventCallback&& func) { closefunc_ = std::move(func); }
  void SetErrorCallback(const EventCallback& func) { errorfunc_ = func; }
  void SetErrorCallback(EventCallback&& func) { errorfunc_ = std::move(func); }

  void EnableRead();
  void EnableWrite();

  void DisableRead();
  void DisableWrite();
  void DisableAll();

  bool IsNoneEvent() const { return events_ == kNoneEvent; }
  bool IsReading() const { return events_ & kReadEvent; }
  bool IsWriting() const { return events_ & kWriteEvent; }

  void RemoveEvents();

  EventLoop* OwnerEventLoop() const  { return eventloop_; }

  void Tie(const std::shared_ptr<void>&);

 private:
  void UpdateEvents();
  void HandleEventWithGuard();

  static const int kNoneEvent = 0;
  static const int kReadEvent = POLLIN | POLLPRI;
  static const int kWriteEvent = POLLOUT;

  EventLoop* eventloop_;
  const int fd_;
  int events_;
  int revents_;
  int index_;

  std::weak_ptr<void> tie_;
  bool tied_;
  bool event_handling_;

  EventCallback readfunc_;
  EventCallback writefunc_;
  EventCallback closefunc_;
  EventCallback errorfunc_;
};

}  // namespace mirants

#endif   // MIRANTS_CORE_DISPATCH_H_
