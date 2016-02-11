#ifndef MIRANTS_CORE_CALLBACK_H_
#define MIRANTS_CORE_CALLBACK_H_

#include <functional>
#include <memory>

namespace mirants {

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*)> MessageCallback;

}  // namespace mirants

#endif  // MIRANTS_CORE_CALLBACK_H_
