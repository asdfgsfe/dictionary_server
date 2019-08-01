#inclue "SendHadler.h"

namespace Sender
{
namespace Frame
{
SendHadler::SendHadler(EventLoop* loop, const string& name)
  : loop_(loop)
    name_(name),
    ioThreadPool_(std::make_shared<EventLoopThreadPool>(loop, name_)),
    nextConnId_(1)
{
}

SendHadler::~SendHadler()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";
  for (auto& peer : peers_)
  {
    PeerPtr newPeer(peer.second);
    peer.second.reset();
    newPeer->getLoop()->runInLoop(std::bind(&Peer::PeerDestroyed, newPeer));
  }
}

void SendHadler::Start()
{
  if (started_.getAndSet(1) == 0)
  {
    ioThreadPool_->start(threadInitCallback_);
    //assert(!acceptor_->listenning()); fix 自己封装个acceptor_仅仅用于拿数据的判断 线程间怎么通信呢？？分配连接
    //loop_->runInLoop(std::bind(&Acceptor::listen, get_pointer(acceptor_))); //运行一个从本地队列哪数据的server OnMessage 
  }                                                          
}

void SendHandler::NewConnection(const Node& node, const InetAddress& peerAddr)
{
  loop_->assertInLoopThread();
  EventLoop* ioLoop = threadPool_->getNextLoop();
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  string connName = name_ + buf;
  LOG_INFO << "TcpServer::newConnection [" << name_
         << "] - new connection [" << connName
                  << "] from " << peerAddr.toIpPort();
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
                  // FIXME poll with zero timeout to double confirm the new connection
                  // FIXME use make_shared if necessary
  peers_[PeerName] = std::make_shared_ptr<Peer>(ioLoop,connName,sockfd,localAddr,peerAddr);
  Peers->SetConnectionCallback(connectionCallback_); //往自己线程tsd插入数据 一个conn只能在一个线程中
  //每个机器销毁自己 这里应该是一个栈对象自己析构 每次发完数据自己析构 或者NodeManager 处理节点的断开与连接 
  //peer对象何时析构是个问题 发完一次数据全部析构还是 只析构变化的节点 
  conn->SetCloseCallback(
      std::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe 
  ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn)); //TODO 会启动connectionCallback_的回调
}

} //namespace Sender
} //namespace Frame
