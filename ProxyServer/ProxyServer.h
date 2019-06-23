#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include <google/protobuf/message.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/base/Mutex.h>

#include "DAL/HiredisPool.h"
#include "ProtobufCodec.h"
#include "Dispatcher.h"
#include "Message/Query.pb.h"
#include "Configure.h"

#include <functional>
#include <map>

typedef std::shared_ptr<muduo::Query> QueryPtr;
typedef std::shared_ptr<muduo::Set> SetPtr;

//using namespace std;

class ProxyServer : muduo::noncopyable
{
public:
    ProxyServer(muduo::net::EventLoop* loop,
                Configure *conf,
                int numThreads);
    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onUnknownMessage(const muduo::net::TcpConnectionPtr& conn,
                          const MessagePtr& message,
                          muduo::Timestamp);

    void onQuery(const muduo::net::TcpConnectionPtr& conn,
                 const QueryPtr& message,
                 muduo::Timestamp);

    void onSet(const muduo::net::TcpConnectionPtr& conn,
               const SetPtr &message,
               muduo::Timestamp);

    void setInLoop(const muduo::net::TcpConnectionPtr& conn, const SetPtr &msg);
    void queryInLoop(const muduo::net::TcpConnectionPtr& conn, const QueryPtr &msg);

    // redis command callbacks
    void commandCallback(db::Hiredis* redis, redisReply* rp, int64_t uniqueConId);

private:
    muduo::net::TcpServer server_;
    int numThreads_;
    std::map<int64_t, muduo::net::TcpConnectionPtr> clientConns_;
    std::mutex mutexutex_;
    db::HiredisPool backend_;
    ProtobufCodec codec_;
    ProtobufDispatcher dispatcher_;
    muduo::net::EventLoop *loop_;
    Configure *conf_;
};

#endif
