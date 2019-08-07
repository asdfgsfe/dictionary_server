#include "ProxyServer.h"
#include "DAL/Hiredis.h"
#include "DAL/HiredisPool.h"

#include <muduo/base/StringPiece.h>
#include <thread>
#include <string>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

std::string toString(long long value)
{
    char buf[32];
    snprintf(buf, sizeof buf, "%lld", value);
    return buf;
}

std::string redisReplyToString(const redisReply* reply)
{
    static const char* const types[] = { "",
                                         "REDIS_REPLY_STRING", "REDIS_REPLY_ARRAY",
                                         "REDIS_REPLY_INTEGER", "REDIS_REPLY_NIL",
                                         "REDIS_REPLY_STATUS", "REDIS_REPLY_ERROR" };
    std::string str;
    if (!reply) return str;

    str += types[reply->type] + std::string("(") + toString(reply->type) + ") ";

    str += "{ ";
    if (reply->type == REDIS_REPLY_STRING ||
            reply->type == REDIS_REPLY_STATUS ||
            reply->type == REDIS_REPLY_ERROR) {
        str += '"' + std::string(reply->str, reply->len) + '"';
    } else if (reply->type == REDIS_REPLY_INTEGER) {
        str += toString(reply->integer);
    } else if (reply->type == REDIS_REPLY_ARRAY) {
        str += toString(reply->elements) + " ";
        for (size_t i = 0; i < reply->elements; i++) {
            str += " " + redisReplyToString(reply->element[i]);
        }
    }
    str += " }";

    return str;
}

void ProxyServer::commandCallback(db::Hiredis* redis, redisReply* rp, int64_t uniqueConId)
{
    std::string reply = redisReplyToString(rp);
    LOG_INFO << "Send to client:" << reply;

    muduo::Reply answer;
    answer.set_reply(reply);

    // find the client conn, and send the reply
    auto it = clientConns_.find(uniqueConId);
    if (it != clientConns_.end()) {
        codec_.send(it->second, answer);
        it->second->shutdown(); // close or not?
    }
}

ProxyServer::ProxyServer(muduo::net::EventLoop *loop,
                         Configure *conf,
                         int numThreads)
    : server_(loop, conf->getListenAddr(), "ProxyServer"), /* ensure conf is not null */
      numThreads_(numThreads),
      dispatcher_(std::bind(&ProxyServer::onUnknownMessage, this, _1, _2, _3)),
      codec_(std::bind(&ProtobufDispatcher::onProtobufMessage, &dispatcher_, _1, _2, _3)),
      backend_(loop, conf),
      loop_(loop),
      conf_(conf)
{
    server_.setThreadNum(numThreads_);

    dispatcher_.registerMessageCallback<muduo::Query>(
                std::bind(&ProxyServer::onQuery, this, _1, _2, _3));
    dispatcher_.registerMessageCallback<muduo::Set>(
                std::bind(&ProxyServer::onSet, this, _1, _2, _3));

    server_.setConnectionCallback(
                std::bind(&ProxyServer::onConnection, this, _1));
    server_.setMessageCallback(
                std::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));
}

void ProxyServer::start()
{
    backend_.connect();
    server_.start();
}

// SET COMMAND
void ProxyServer::setInLoop(const muduo::net::TcpConnectionPtr& conn, const SetPtr& msg)
{
    const std::string& key = msg->key();
    db::HiredisPtr redis = backend_.getHiredis(key.c_str(), key.size());
    if (redis) {
        int32_t serverId = redis->getServerId();
        int32_t conId = -1;
        std::string cmd("set ");
        cmd += key + std::string(" ") + msg->value();
        redis->command(&conId,
                       std::bind(&ProxyServer::commandCallback, this, _1, _2, _3),
                       cmd.c_str());
        clientConns_[db::genUniqueConId(serverId, conId)] = conn;
    } else {
        LOG_WARN << "No redis connection available.";
    }
}

// GET COMMAND
void ProxyServer::queryInLoop(const muduo::net::TcpConnectionPtr& conn,
                              const QueryPtr& msg)
{
    const std::string& key = msg->key();
    db::HiredisPtr redis = backend_.getHiredis(key.c_str(), key.size());
    if (redis) {
        int32_t serverId = redis->getServerId();
        int32_t conId = -1;
        std::string cmd("get ");
        cmd += key;
        redis->command(&conId,
                       std::bind(&ProxyServer::commandCallback, this, _1, _2, _3),
                       cmd.c_str());
        clientConns_[db::genUniqueConId(serverId, conId)] = conn;
    } else {
        LOG_WARN << "No redis connection available.";
    }
}

void ProxyServer::onQuery(const muduo::net::TcpConnectionPtr& conn,
                          const QueryPtr& message,
                          muduo::Timestamp)
{
    LOG_INFO << "onQuery:\n" << message->GetTypeName() << message->DebugString();

    loop_->runInLoop(std::bind(&ProxyServer::queryInLoop, this, conn, std::ref(message)));
}

void ProxyServer::onSet(const muduo::net::TcpConnectionPtr& conn,
                        const SetPtr& message,
                        muduo::Timestamp)
{
    LOG_INFO << "onSet:\n" << message->GetTypeName() << message->DebugString();

    loop_->runInLoop(std::bind(&ProxyServer::setInLoop, this, conn, std::ref(message)));
}

void ProxyServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
             << conn->peerAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

void ProxyServer::onUnknownMessage(const muduo::net::TcpConnectionPtr& conn,
                                   const MessagePtr& message,
                                   muduo::Timestamp)
{
    LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
}
