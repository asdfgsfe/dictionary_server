#ifndef PROXY_SERVER_HIREDIS_H
#define PROXY_SERVER_HIREDIS_H

#include "muduo/base/noncopyable.h"
#include "muduo/base/StringPiece.h"
#include "muduo/base/Types.h"
#include "muduo/net/Callbacks.h"
#include "muduo/net/InetAddress.h"

#include <hiredis/hiredis.h>
#include <boost/any.hpp>
#include <list>
#include <map>

struct redisAsyncContext;

namespace muduo
{
namespace net
{
class Channel;
class EventLoop;
}
}

namespace db
{

static int64_t genUniqueConId(int32_t serverId, int32_t conId)
{
    return (((int64_t)serverId << 32) | conId);
}

class Hiredis : public std::enable_shared_from_this<Hiredis>, muduo::noncopyable
{
public:
    typedef std::function<void(Hiredis*, int)> ConnectCallback;
    typedef std::function<void(Hiredis*, int)> DisconnectCallback;
    typedef std::function<void(Hiredis*, redisReply*, int64_t)> CommandCallback;

    Hiredis(muduo::net::EventLoop* loop,
            const muduo::net::InetAddress& serverAddr,
            int nconns);
    ~Hiredis();

    const muduo::net::InetAddress& serverAddress() const { return serverAddr_; }
    redisAsyncContext* context() { return context_; }
    int32_t getServerId() const { return serverId_; }
    void setServerId(int id) { serverId_ = id; }
    bool connected() const;
    bool connected(const redisAsyncContext* ac) const;
    const char* errstr() const;

    void setConnectCallback(const ConnectCallback& cb) { connectCb_ = cb; }
    void setDisconnectCallback(const DisconnectCallback& cb) { disconnectCb_ = cb; }

    void connect();
    void disconnect();  // FIXME: implement this with redisAsyncDisconnect

    int command(int32_t *id, const CommandCallback& cb, muduo::StringArg cmd, ...);

    int ping();

    // not used
    void setUserContext(const boost::any& context) { userContext_ = context; }
    const boost::any& getUserContext() const { return userContext_; }
    boost::any* getMutableUserContext() { return &userContext_; }

private:
    struct ServerConn {
        ServerConn() : connId(-1), context(NULL), hiredis(NULL) {}
        explicit ServerConn(redisAsyncContext* c, Hiredis* h)
            : connId(-1), context(c), hiredis(h) {}

        int32_t connId;
        Hiredis* hiredis;
        redisAsyncContext* context;
        std::shared_ptr<muduo::net::Channel> channel;
    };

    void handleRead(ServerConn *scon, muduo::Timestamp receiveTime);
    void handleWrite(ServerConn* scon);

    int fd(const redisAsyncContext *ac) const;
    void logConnection(const redisAsyncContext* ac, bool up) const;
    void setChannel(ServerConn *scon);
    void removeChannel(ServerConn *scon);

    void userConnectCallback(const redisAsyncContext *ac, int status);
    void userDisconnectCallback(const redisAsyncContext* ac, int status);
    void commandCallback(redisAsyncContext *ac, redisReply* reply, CommandCallback* privdata);

    static Hiredis* getHiredis(const redisAsyncContext* ac);

    static void connectCallback(const redisAsyncContext* ac, int status);
    static void disconnectCallback(const redisAsyncContext* ac, int status);

    // callbacks in c-style
    static void commandCallback(redisAsyncContext* ac, void*, void*);
    static void addRead(void* privdata);
    static void delRead(void* privdata);
    static void addWrite(void* privdata);
    static void delWrite(void* privdata);
    static void cleanup(void* privdata);

    void pingCallback(Hiredis* me, redisReply* reply);

private:
    int32_t serverId_;
    muduo::net::EventLoop* loop_;
    const muduo::net::InetAddress serverAddr_;
    redisAsyncContext* context_;
    std::shared_ptr<muduo::net::Channel> channel_;

    /// support multi server connction
    int nconns_;
    std::map<int, ServerConn*> inactiveConns_;
    std::map<int, ServerConn*> activeFreeConns_;
    std::map<int, ServerConn*> activeBusyConn_;

    ConnectCallback connectCb_;
    DisconnectCallback disconnectCb_;
    boost::any userContext_;
};

}  // namespace hiredis

#endif
