#include <stdio.h>

#include <boost/bind.hpp>

#include "DicClient.h"

Client::Client(EventLoop* loop, const InetAddress& serverAddr)
  : loop_(loop),
    client_(loop, serverAddr, "Client"),
    codec_(boost::bind(&Client::onStringMessage, this, _1, _2, _3))
  {
    client_.setConnectionCallback(
        boost::bind(&Client::onConnection, this, _1));
    client_.setMessageCallback(
        boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    client_.enableRetry();
  }

~Client::Client(){}

// 该函数在主线程中执行
void Client::write(const StringPiece& message)
{
  // mutex用来保护connection_这个shared_ptr
  MutexLockGuard lock(mutex_);
  if (connection_)
  {
    codec_.send(get_pointer(connection_), message);
  }
}

  // 该函数在IO线程中执行，IO线程与主线程不在同一个线程
void Client::onConnection(const TcpConnectionPtr& conn)
{
  LOG_INFO << conn->localAddress().toIpPort() << " -> "
           << conn->peerAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");

  // mutex用来保护connection_这个shared_ptr
  MutexLockGuard lock(mutex_);
  if (conn->connected())
  {
    connection_ = conn;
  }
  else
  {
    connection_.reset();
  }
}

void Client::onStringMessage(const TcpConnectionPtr&,
                             const string& message,
                             Timestamp)
{
  printf("<<< %s\n", message.c_str());
}
