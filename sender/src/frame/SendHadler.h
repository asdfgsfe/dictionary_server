#ifndef _CONNECT_POOL_H_
#define _CONNECT_POOL_H_

#include <vector>
#include <momory>
#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include "DictClient.h"
#include "Node.h"

namespace Sender
{
namespace Frame
{

class SendHadler : boost::noncopyable
{
  using PeerPtr = std::shared_ptr<Peer>;
  using NodePtr = std::shared_ptr<Node>;
  using PeerList  = std::set<PeerPtr> PeerList;
  using LocalPeer = muduo::ThreadLocalSingleton<PeerList>;
  using PeerMap = std::unordered_map<NodePtr, PeerPtr>; //node->peer
public:
  SendHadler(const string& name = "SendHadler");
  ~SendHadler() = default; //fix
  void Start();
  void Stop();
  void PutData(muduo::net::Buffer&& data)
  muduo::net::Buffer TakeData();
  
  void SetThreadNum(uint16_t numThreads) 
  { threadPool_->setThreadNum(numThreads); }
  
  void SetThreadInitCallback(const ThreadInitCallback& cb)
  { threadInitCallback_ = cb; }
  
  void SetMessageCallback(const MessageCallback& cb) //session回调 应该设置成这个回调dispatc::hSendData
  { messageCallback_ = cb; }
  
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }
  
  void SetConnectionCallback(const ConnectionCallback& cb)//nodeManeger回调
  { ConnectionCallback_ = cb; }

private:
  /// Not thread safe, but in loop
  void NewConnection(const Node& node);
  /// Thread safe.
  void RemoveConnection(const PeerPtr& peer);
  /// Not thread safe, but in loop
  void RemoveConnectionInLoop(const PeerPtr& peer);

private:
  EventLoop* loop_;  // the acceptor loop == start loop 外面线程的eventloop
  const string name_;
  std::shared_ptr<EventLoopThreadPool> ioThreadPool_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  ThreadInitCallback threadInitCallback_;
  AtomicInt32 started_;
  // always in loop thread
  int nextConnId_;
  PeerMap peers_;
};

} //namespace Frame
} //namespace Sender

#endif //_CONNECT_POOL_H_

