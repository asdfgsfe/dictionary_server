#ifndef _CONN_POOL_H_
#define _CONN_POOL_H_

#include <vector>
#include <memory>

#include <boost/noncopyable.hpp>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThreadPool.h>
#include <muduo/net/TcpClient.h>

namespace Sender
{
namespace Frame
{
class DictClient;
enum EState {IDLEING, TASKING}
class Peer : boost::noncopyable,
             public std::enable_shared_from_this<Peer>
{
  using ClientPtr = std::unique_ptr<DictClient>;
  using ClientList = std::vector<ClientPtr>;
public:
  Peer(uint32_t maxConnections, uint32_t numThreads);
  ~Peer() = default;
  void Start();
  void ResetConnect(const InetAddress& newAddr, bool nodelay);  
  bool State(EState state) const { return state_; }
  void SetState(EState state) { state_ = state; }
  void Send();

  void ConnectEstablished();
  void ConnectDestroyed();
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }
  
  void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) //队列大小的限制
  {
     highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }
  }
  void setCloseCallback(const CloseCallback& cb)
  { closeCallback_ = cb; }

private:
  void ConnectPeer();

private:
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
  EventLoop* loop_;
  muduo::net::EventLoopThreadPool loopPool_;
  muduo::BlockingQueue<std::string> datas_;
  boost::any context_; //暂时留着
  const string name_;
  const InetAddress localAddr_;
  const InetAddress peerAddr_;
  StateE state_;  // FIXME: use atomic variable
  ConnectionCallback connectionCallback_;
  HighWaterMarkCallback highWaterMarkCallback_; //限制队列大小的
  CloseCallback closeCallback_; //析构所有的连接和线程池
  size_t highWaterMark_;
  const uint32_t maxConnections_;
  uint32_t aliveConnections_;
  ClientPtrList clients_;
  bool nodelay_;
};

} //namespace Frame
} //namespace sender

#endif //_CONN_POOL_H_
