#ifndef _CONNECT_POOL_H_
#define _CONNECT_POOL_H_

#include <vector>
#include <momory>
#include <string>
#include <boost/noncopyable.hpp>
#include <muduo/base/BlockingQueue.h>
#include "DictClient.h"

namespace Sender
{
namespace Frame
{

class DispatchServer : boost::noncopyable
{
  using PeerPtr = std::shared_ptr<Peer>;
  using PeerList  = std::set<PeerPtr> ConnectionList;
  using LocalPeers = muduo::ThreadLocalSingleton<PeerList>;
public:
  DispatchServer(const string& name = "DispatchServer");
  ~DispatchServer() = default; //fix
  void Start(int size); //启动tcpserver的线程池
  void Stop();
  void ThreadInit(EventLoop* loop);
  void SendData(string&& data, muduo::Timestamp receiveTime);
  void OnConnection(const PeerPtr& peer);
  void OnMessage();

  void Send(const PeerPtr& peer, const muduo::StringPiece& message);
  void DistributeMessage(const string& data);
  void OnStringMessage(const string& message, Timestamp);

  void setThreadNum(int numThreads) { server_.setThreadNum(numThreads); }

private:
  std::string name_;
  bool running_;
  NodeManager& nodes_;

  //DispatchServer == chatServer
  SendHandler sender_;
  SessionHandler session_;
  MutexLock mutex_;                                             
  std::set<EventLoop*> loops_ GUARDED_BY(mutex_); //EventLoop 栈对象
  //thread safe
  muduo::BlockingQueue<std::string> datas_;
};

} //namespace Frame
} //namespace Sender

#endif //_CONNECT_POOL_H_
