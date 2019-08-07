#inclue "DispatchServer.h"

namespace Sender
{
namespace Frame
{
DispatchServer::DispatchServer(const string& name)
  : name_(name),
    running_(false),
    peers_()
{
  sender_.SetConnectionCallback(
          std::bind(&DispatchServer::OnConnection, this, _1));
  session_.SetMessageCallback(
          std::bind(&DispatchServer::SendData, this, _1, _2));
}

DispatchServer::~DispatchServer()
{
  if (running_) { Stop(); }
}

void DispatchServer::Start(int size)
{
  server_.setThreadInitCallback(std::bind(&DispatchServer::threadInit, this, _1));
  server_.start(size);
  running_ = true;
}

void DispatchServer::OnConnection(const PeerPtr& peer) //ConnectPoolPtr==peer
{
  //fix 这个地方只有insert操作 将所有的peer放到每个线程tsd中 这些线程将数据发到每个peer中的队列中
  //外面for (peers) insert到此处
  LOG_INFO << peer->localAddress().toIpPort() << " -> "
    << peer->peerAddress().toIpPort() << " is "
    << (peer->connected() ? "UP" : "DOWN");
  LocalPeers::instance().insert(peer);
}

//已经将数据发送到peer里面了 //for (dicts) 遍历发送多块数据
void DispatchServer::OnStringMessage(const string& message, Timestamp)
{
  Peer::Functor f = std::bind(&DispatchServer::DistributeMessage, this, message);
  LOG_DEBUG;
  MutexLockGuard lock(mutex_);
  for (auto loop : loops_) //保证将每个线程值发自己拿到的数据
  {
    loop->queueInPeer(f);
    //loop->runInPeer(f);
  }
  LOG_DEBUG;
}


void DispatchServer::DistributeMessage(const string& data)
{
  LOG_DEBUG << "DistributeDict begin";
  for (auto peer : LocalPeers) //将data发送给每一个peer
  {
    Send(peer, data); //conn指tcpConn ==应该扔到每个机器(peer)的队列中 peer在调用client发送 loadtest
  }
  LOG_DEBUG << "end";
}

//TODO OnMessage接口自己内部的线程调用 本质内部的线程池等待到条件变量上
//fix 此处用eventfd 来通信
void DispatchServer::OnMessage() //OnMessage
{
  //for (dicts) 遍历发送多块数据
  while (running_ && !datas_.empty())
  {
    OnStringMessage(datas_.take(), Timestamp());
  }
}

void DispatchServer::ThreadInit(EventLoop* loop)
{
  assert(!LocalPeers::pointer());
  LocalPeers::instance();
  assert(LocalPeers::pointer());
  MutexLockGuard lock(mutex_);
  loops_.emplace(loop);
}

//session调用这个接口r扔到队列中   一个session负责一个词典 可以考虑丢弃 通过让session回调onMeaasge解决 
void DispatchServer::SendData(string&& data, muduo::Timestamp receiveTime)
{
  if (running_)
  {
    datas_.put(std::forward(data));
  }
}

void Send(const PeerPtr& peer, const muduo::StringPiece& message)
{
  muduo::net::Buffer buf;
  buf.append(message.data(), message.size());
  int32_t len = static_cast<int32_t>(message.size());
  int32_t be32 = muduo::net::sockets::hostToNetwork32(len); 
  buf.prepend(&be32, sizeof be32);
  peer->Send(message);
}

} //namespace Sender
} //namespace Frame
