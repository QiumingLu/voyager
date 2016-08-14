#ifndef VOYAGER_CORE_DISPATCH_H_
#define VOYAGER_CORE_DISPATCH_H_

#include <functional>
#include <memory>
#include <sys/poll.h>

namespace voyager {

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

  void SetReadCallback(const EventCallback& cb) { read_cb_ = cb; }
  void SetWriteCallback(const EventCallback& cb) { write_cb_ = cb; }
  void SetCloseCallback(const EventCallback& cb) { close_cb_ = cb; }
  void SetErrorCallback(const EventCallback& cb) { error_cb_ = cb; }
  
  void SetReadCallback(EventCallback&& cb) { read_cb_ = std::move(cb); }
  void SetWriteCallback(EventCallback&& cb) { write_cb_ = std::move(cb); }
  void SetCloseCallback(EventCallback&& cb) { close_cb_ = std::move(cb); }
  void SetErrorCallback(EventCallback&& cb) { error_cb_ = std::move(cb); }

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

  void Tie(const std::shared_ptr<void>& obj);

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

  EventCallback read_cb_;
  EventCallback write_cb_;
  EventCallback close_cb_;
  EventCallback error_cb_;

  // No copying allow
  Dispatch(const Dispatch&);
  void operator=(const Dispatch&);
};

}  // namespace voyager

#endif   // VOYAGER_CORE_DISPATCH_H_
