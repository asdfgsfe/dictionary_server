#ifndef _DIC_CLIENT_H_
#define _DIC_CLIENT_H_

#include <muduo/base/Mutex.h>
#include <muduo/net/TcpClient.h>

#include <boost/noncopyable.hpp>

#include "codec.h"

using namespace muduo;
using namespace muduo::net;

class Client : boost::noncopyable
{
  using DataCallback = std::function(muduo::StringPiece message ());
public:
  Client(EventLoop* loop, const InetAddress& serverAddr);
  ~Client() = default;

  void connect()
  {
    client_.connect();
  }

  void disconnect()
  {
    client_.disconnect();
  }

  // 该函数在主线程中执行
  void OnHighWaterMark();
  void OnWriteComplete();
  void SetDataCallback(const DataCallback& cb) 
  { dataCallback_ = cb; }
 private:
  // 该函数在IO线程中执行，IO线程与主线程不在同一个线程
  void OnConnection(const TcpConnectionPtr& conn);
  //TODO debug == recv
  void OnStringMessage(const TcpConnectionPtr&,
                       const string& message,
                       Timestamp);
  void Send();
private:
  EventLoop* loop_;
  TcpClient client_;
  LengthHeaderCodec codec_;
  TcpConnectionPtr connection_;
  DataCallback dataCallback_;
};

#endif //_DIC_CLIENT_H_
