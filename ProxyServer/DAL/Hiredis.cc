#include "Hiredis.h"
#include "muduo/base/Logging.h"
#include "muduo/net/Channel.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/SocketsOps.h"

#include <hiredis/async.h>

using namespace muduo;
using namespace muduo::net;
using namespace db;

static void dummy(const std::shared_ptr<Channel>&)
{
}

Hiredis::Hiredis(EventLoop* loop, const InetAddress& serverAddr, int nconns)
    : serverId_(-1),
      loop_(loop),
      serverAddr_(serverAddr),
      context_(nullptr),
      nconns_(nconns)
{
}

Hiredis::~Hiredis()
{
    LOG_DEBUG << this;
    assert(!channel_ || channel_->isNoneEvent());
    ::redisAsyncFree(context_);
    //    TODO: free scon
    //    for (const auto& sconPair : freeConns_) {
    //        ::redisAsyncFree(sconPair->second->context);
    //    }
}

bool Hiredis::connected() const
{
    return !(activeBusyConn_.empty() && activeFreeConns_.empty());
}

bool Hiredis::connected(const redisAsyncContext* ac) const
{
    if (ac != NULL) {
        const ServerConn* scon = static_cast<const ServerConn*>(ac->ev.data);
        return scon->channel && (ac->c.flags & REDIS_CONNECTED);
    }
    return false;
}

// TODO: Modify
const char* Hiredis::errstr() const
{
    assert(context_ != NULL);
    return context_->errstr;
}

void Hiredis::connect()
{
    assert(!context_);

    /// support multi-conn
    assert(inactiveConns_.empty());
    redisAsyncContext* context = NULL;
    for (int i = 0; i < nconns_; ++i) {
        LOG_INFO << "conn id: " << i;
        context = ::redisAsyncConnect(serverAddr_.toIp().c_str(), serverAddr_.toPort());
        ServerConn* scon = new ServerConn(context, this);
        scon->connId = i;
        context->ev.addRead = addRead;
        context->ev.delRead = delRead;
        context->ev.addWrite = addWrite;
        context->ev.delWrite = delWrite;
        context->ev.cleanup = cleanup;
        context->ev.data = scon; // privdata

        setChannel(scon);

        assert(context->onConnect == NULL);
        assert(context->onDisconnect == NULL);
        ::redisAsyncSetConnectCallback(context, connectCallback);
        ::redisAsyncSetDisconnectCallback(context, disconnectCallback);

        inactiveConns_[scon->connId] = scon;
    }
}

// TODO
void Hiredis::disconnect()
{
    if (connected())
    {
        LOG_DEBUG << this;
//        for (auto it = activeFreeConns_.begin(); it != activeFreeConns_.end();
//             ++it) {
//            ::redisAsyncDisconnect(it->second->context);
//        }
//        for (auto it = activeBusyConn_.begin(); it != activeBusyConn_.end();
//             ++it) {
//            ::redisAsyncDisconnect(it->second->context);
//        }
        ::redisAsyncDisconnect(context_);
    }
}

int Hiredis::fd(const redisAsyncContext* ac) const
{
    assert(ac != NULL);
    return ac->c.fd;
}

void Hiredis::setChannel(ServerConn* scon)
{
    LOG_DEBUG << this;
    assert(scon != nullptr);
    assert(!scon->channel);
    scon->channel.reset(new Channel(loop_, fd(scon->context)));
    scon->channel->setReadCallback(std::bind(&Hiredis::handleRead, this, scon, _1));
    scon->channel->setWriteCallback(std::bind(&Hiredis::handleWrite, this, scon));
}

void Hiredis::removeChannel(ServerConn* scon)
{
    LOG_DEBUG << "remove channel: " << this;
    scon->channel->disableAll();
    scon->channel->remove();
    loop_->queueInLoop(std::bind(dummy, channel_));
    scon->channel.reset();
}

void Hiredis::handleRead(ServerConn* scon, muduo::Timestamp receiveTime)
{
    assert(scon != NULL);
    LOG_TRACE << "receiveTime = " << receiveTime.toString();
    ::redisAsyncHandleRead(scon->context);
}

void Hiredis::handleWrite(ServerConn* scon)
{
    assert(scon->context != NULL);
    if (!(scon->context->c.flags & REDIS_CONNECTED)) {
        removeChannel(scon);
        LOG_DEBUG << ">>remove Channel.";
    }
    ::redisAsyncHandleWrite(scon->context);
}

/* static */ Hiredis* Hiredis::getHiredis(const redisAsyncContext* ac)
{
    ServerConn* scon = static_cast<ServerConn*>(ac->ev.data);
    assert(scon->context == ac);
    return static_cast<Hiredis*>(scon->hiredis);
}

void Hiredis::logConnection(const redisAsyncContext* ac, bool up) const
{
    InetAddress localAddr(sockets::getLocalAddr(fd(ac)));
    InetAddress peerAddr(sockets::getPeerAddr(fd(ac)));

    LOG_INFO << localAddr.toIpPort() << " -> "
             << peerAddr.toIpPort() << " is "
             << (up ? "UP" : "DOWN");
}

/* static */ void Hiredis::connectCallback(const redisAsyncContext* ac, int status)
{
    LOG_TRACE;
    LOG_INFO << "hiredis connected";
    getHiredis(ac)->userConnectCallback(ac, status);
}

void Hiredis::userConnectCallback(const redisAsyncContext* ac, int status)
{
    ServerConn* scon = static_cast<ServerConn*>(ac->ev.data);
    if (status != REDIS_OK) {
        LOG_ERROR << scon->context->errstr << " failed to connect to " << serverAddr_.toIpPort();
    } else {
        LOG_INFO << "Move to active map";
        logConnection(ac, true);
        auto it = inactiveConns_.find(scon->connId);
        assert(it != inactiveConns_.end());
        activeFreeConns_[it->first] = it->second;
        inactiveConns_.erase(it);

        setChannel(scon);
    }

    if (connectCb_) {
        connectCb_(this, status);
    }
}

/* static */ void Hiredis::disconnectCallback(const redisAsyncContext* ac, int status)
{
    LOG_TRACE;
    LOG_INFO << ">>Disconnected";
    getHiredis(ac)->userDisconnectCallback(ac, status);
}

void Hiredis::userDisconnectCallback(const redisAsyncContext* ac, int status)
{
    logConnection(ac, false);
    removeChannel(static_cast<ServerConn*>(ac->ev.data));

    // move to freeConn_
    ServerConn* scon = static_cast<ServerConn*>(ac->ev.data);
    auto it = activeBusyConn_.find(scon->connId);
    assert(it != activeBusyConn_.end());
    assert(inactiveConns_.find(it->first) == inactiveConns_.end());
    inactiveConns_[it->first] = it->second;
    activeBusyConn_.erase(it);

    if (disconnectCb_) {
        disconnectCb_(this, status);
    }
}

/*static*/void Hiredis::addRead(void* privdata)
{
    LOG_TRACE;
    ServerConn* scon = static_cast<ServerConn*>(privdata);
    scon->channel->enableReading();
}

void Hiredis::delRead(void* privdata)
{
    LOG_TRACE;
    ServerConn* scon = static_cast<ServerConn*>(privdata);
    scon->channel->disableReading();
}

void Hiredis::addWrite(void* privdata)
{
    LOG_TRACE;
    ServerConn* scon = static_cast<ServerConn*>(privdata);
    scon->channel->enableWriting();
}

void Hiredis::delWrite(void* privdata)
{
    LOG_TRACE;
    ServerConn* scon = static_cast<ServerConn*>(privdata);
    scon->channel->disableWriting();
}

void Hiredis::cleanup(void* privdata)
{
    ServerConn* scon = static_cast<ServerConn*>(privdata);
    LOG_DEBUG << scon;
}

int Hiredis::command(int32_t *connId, const CommandCallback& cb, muduo::StringArg cmd, ...)
{
    assert(connId != nullptr);
    if (!connected()) return REDIS_ERR;
    int ret = -1;
    *connId = -1;

    // select a active conn, and move to busyConn_
    if (!activeFreeConns_.empty()) {
        auto it = activeFreeConns_.begin();
        ServerConn * scon = it->second;
        *connId = scon->connId;
        activeFreeConns_.erase(it);
        activeBusyConn_[scon->connId] = scon;

        LOG_TRACE;
        CommandCallback* p = new CommandCallback(cb); // cb is user callback
        LOG_INFO << "Redis command: " << cmd.c_str();
        va_list args;
        va_start(args, cmd);
        ret = ::redisvAsyncCommand(scon->context, commandCallback, p, cmd.c_str(), args);
        va_end(args);
    } else {
        LOG_WARN << "No active connection to use.";
    }

    return ret;
}

/* static */ void Hiredis::commandCallback(redisAsyncContext* ac, void* r, void* privdata)
{
    redisReply* reply = static_cast<redisReply*>(r);
    CommandCallback* cb = static_cast<CommandCallback*>(privdata);
    getHiredis(ac)->commandCallback(ac, reply, cb);
}

void Hiredis::commandCallback(redisAsyncContext* ac, redisReply* reply, CommandCallback* cb)
{
    // cb is user callback!
    ServerConn* scon = static_cast<ServerConn*>(ac->ev.data);
    (*cb)(this, reply, genUniqueConId(serverId_, scon->connId));
    delete cb;

    auto it = activeBusyConn_.find(scon->connId);
    assert(it != activeBusyConn_.end());
    assert(activeFreeConns_.find(it->first) == activeFreeConns_.end());
    activeFreeConns_[it->first] = it->second;
    activeBusyConn_.erase(it);
}

//int Hiredis::ping()
//{
//    return command(std::bind(&Hiredis::pingCallback, this, _1, _2, _3), "PING");
//}

//void Hiredis::pingCallback(Hiredis* me, redisReply* reply, int64_t )
//{
//    assert(this == me);
//    LOG_DEBUG << reply->str;
//}
