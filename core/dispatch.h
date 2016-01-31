#ifndef MIRANTS_CORE_DISPATCH_H_
#define MIRANTS_CORE_DISPATCH_H_

#include <functional>
#include <sys/poll.h>
namespace mirants {

class EventLoop;
class Dispatch {
 public:
  typedef std::function<void()> EventCallBack;

  Dispatch(EventLoop* eventloop, int fd);
  ~Dispatch();

  int Fd() const { return fd_; }
  int Events() const { return events_; }
  void SetRevents(int rv) { revents_ = rv; }

  void HandleEvent();

  void SetReadCallBack(const EventCallBack&& func) { readfunc_ = func; }
  void SetReadCallBack(EventCallBack&& func) { readfunc_ = std::move(func); }
  void SetWriteCallBack(const EventCallBack& func) { writefunc_ = func; }
  void SetWriteCallBack(EventCallBack&& func) { writefunc_ = std::move(func); }
  void SetCloseCallBack(const EventCallBack& func) { closefunc_ = func; }
  void SetCloseCallBack(EventCallBack&& func) { closefunc_ = std::move(func); }
  void SetErrorCallBack(const EventCallBack& func) { errorfunc_ = func; }
  void SetErrorCallBack(EventCallBack&& func) { errorfunc_ = std::move(func); }

  void EnableRead();
  void EnableWrite();

  void DisableRead();
  void DisableWrite();
  void DisableAll();

  bool IsReading() const { return events_ & kReadEvent; }
  bool IsWriting() const { return events_ & kWriteEvent; }

 private:
  void UpdateEvent();

  static const int kNoneEvent = 0;
  static const int kReadEvent = POLLIN | POLLPRI;
  static const int kWriteEvent = POLLOUT;

  EventLoop* eventloop_;
  const int fd_;
  int events_;
  int revents_;

  EventCallBack readfunc_;
  EventCallBack writefunc_;
  EventCallBack closefunc_;
  EventCallBack errorfunc_;
};

}  // namespace mirants

#endif   // MIRANTS_CORE_DISPATCH_H_
