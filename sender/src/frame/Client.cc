#include <stdio.h>

#include <boost/bind.hpp>

#include "DicClient.h"

//fix name="DictClient:" + serverAddr.IP() + ":" + serverAddr.port""""
Client::Client(EventLoop* loop, const InetAddress& serverAddr, const std::string& name)
  : loop_(loop),
    client_(loop, serverAddr, name),
    codec_(boost::bind(&Client::onStringMessage, this, _1, _2, _3))
  {
    client_.setConnectionCallback(
        boost::bind(&Client::onConnection, this, _1));
    client_.setMessageCallback(
        boost::bind(&LengthHeaderCodec::onMessage, &codec_, _1, _2, _3));
    client_.enableRetry();
  }

  // 该函数在IO线程中执行，IO线程与主线程不在同一个线程
void Client::OnConnection(const TcpConnectionPtr& conn)
{
  LOG_INFO << conn->localAddress().toIpPort() << " -> "
           << conn->peerAddress().toIpPort() << " is "
           << (conn->connected() ? "UP" : "DOWN");

  if (conn->connected())
  {
    connection_ = conn;
  }
  else
  {
    connection_.reset();
  }
  if (connection_)
  {
    loop_->runAfter(2.0, std::bind(&Client::send, this));
  }
}

// FIXME: TcpConnectionPtr
void Send()
{
  assert(dataCallback_);
  const muduo::StringPiece message = dataCallback_();
  codec_->Send(connection_);
}                                                           

void Client::onStringMessage(const TcpConnectionPtr&,
                             const string& message,
                             Timestamp)
{
  printf("<<< %s\n", message.c_str());
}

//fix 一次拿取一部分数据 从hdfs
void Client::OnWriteComplete()
{
  const FilePtr& fp = boost::any_cast<const FilePtr&>(connection_->getContext());
  char buf[kBufSize];
  size_t nread = ::fread(buf, 1, sizeof buf, get_pointer(fp));
  if (nread > 0)
  {
    connection_->send(buf, static_cast<int>(nread));
  }
  else
  {
    connection_->shutdown();
    LOG_INFO << "FileServer - done";
  }
}
