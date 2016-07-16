#ifndef VOYAGER_CORE_CALLBACK_H_
#define VOYAGER_CORE_CALLBACK_H_

#include <functional>
#include <memory>

namespace voyager {

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> DisConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*)> MessageCallback;
typedef std::function<void ()> TimeProcCallback;

}  // namespace voyager

#endif  // VOYAGER_CORE_CALLBACK_H_
