#ifndef MIRANTS_CORE_DISPATCH_H_
#define MIRANTS_CORE_DISPATCH_H_

#include <functional>

namespace mirants {

class EventLoop;
class Dispatch {
 public:
  typedef std::function<void()> EventCallBack;

  Dispatch(EventLoop* eventloop, int fd);
  ~Dispatch();

  int Fd() const { return fd_; }
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

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop* eventloop_;
  const int fd_;
  int events_;
  int revents_;

  EventCallBack readfunc_;
  EventCallBack writefunc_;
  EventCallBack closefunc_;
  EventCallBack errorfunc_;
};

const int Dispatch::kNoneEvent = 0;
const int Dispatch::kReadEvent = POLLIN | POLLPRI;
const int Dispatch::kWriteEvent = POLLOUT;

}  // namespace mirants

#endif   // MIRANTS_CORE_DISPATCH_H_