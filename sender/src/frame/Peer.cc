#include <muduo/base/FileUtil.h>

#include "Peer.h"

namespace Dictionary
{
namespace Frame
{
Peer::Peer(EventLoop* loop,
          const string& nameArg,
          const InetAddress& localAddr,
          const InetAddress& peerAddr,
          uint32_t maxConnections, 
          uint32_t numThreads,
          bool nodelay)
  : loop_(loop),
    loopPool_(loop_, "PeerConnectsOwer"),
    datas(),
    name_(nameArg),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    maxConnections_(maxConnections), 
    numThreads_(numThreads),
    aliveConnections_(0),
    clients_(maxConnections),
    nodelay_(nodelay),
{
}

Peer::~Peer()
{
  LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
    << " fd=" << channel_->fd() << " state=" << stateToString();
  assert(state_ == kDisconnected);
}

//fix 做事情
void ThreadInitCallback()
{
}

void SendHadler::Start()
{
  if (started_.getAndSet(1) == 0) //atomic
  {
    loopPool_.start(threadInitCallback_);
    loopPool.setThreadNum(threads);
    loopPool.start();
    ConnectPeer();
  }                                                          
}

void Peer::ConnectPeer(void)
{
  for (uint32_t i = 0; i < maxConnections_; ++i)
  {
    Fmt f("c%04d", i+1);
    string name(f.data(), f.length());
    clients_.emplace_back(new DictClient(loopPool_.getNextLoop(), 
                                         serverAddr_, 
                                         name, 
                                         nodelay_));
    clients_.back()->connect();
  }
}

//TODO fix peer不析构 内部仅仅替换client对象
void Peer::ResetConnect(const InetAddress& newAddr, bool nodelay)
{
  if (state_ != IDLEING)
  {
    LOG_WARN << "connect poll is tasking, can't reset conn!" ;
    return;
  }
  serverAddr_ = newAddr;
  RunClient(nodelay);
}

//NodeManager 调用这个函数插入节点
void TcpConnection::ConnectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  connectionCallback_(shared_from_this());
}

//自己的析构函数调用
void TcpConnection::ConnectDestroyed()
{
  loop_->assertInLoopThread();
  if (state_ == kConnected)
  {
    setState(kDisconnected);
    connectionCallback_(shared_from_this());
  }
}

} //namespace Dictionary
} //namespace Frame

