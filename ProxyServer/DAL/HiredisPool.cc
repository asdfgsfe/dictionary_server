#include "HiredisPool.h"
#include <muduo/base/Logging.h>
#include <stdarg.h>
#include <functional>

using namespace db;

void connectCallback(db::Hiredis* c, int status)
{
    if (status != REDIS_OK) {
        LOG_ERROR << "connectCallback Error:" << c->errstr();
    } else {
        // TODO: send pending msg?
        LOG_INFO << "Connected...";
    }
}

void disconnectCallback(db::Hiredis* c, int status)
{
    if (status != REDIS_OK) {
        LOG_ERROR << "disconnectCallback Error:" << c->errstr();
    } else {
        //TODO:...
        LOG_INFO << "Disconnected.";
    }
}

HiredisPool::HiredisPool(muduo::net::EventLoop* loop, Configure* conf)
    : loop_(loop),
      conf_(conf)
{
    assert(conf_ != nullptr);
    assert(loop_ != nullptr);

    hashFun_ = hash_fnv1_64;
    serverAddrs_ = conf_->getServerAddrs();
    poolSize_ = serverAddrs_.size(); // TODO: not all server is active.

    assert(!serverAddrs_.empty());

    int perServerConn = conf_->getPerServerConnNum();
    int i = 0;
    for (const auto& addr : serverAddrs_) {
        clients_.emplace_back(new Hiredis(loop_, addr, perServerConn));
        clients_.back()->setServerId(i++);
    }
}

void HiredisPool::setConnectCallback(const db::Hiredis::ConnectCallback& cb)
{
    for (const auto& ptr : clients_) {
        ptr->setConnectCallback(cb);
    }
}

void HiredisPool::setDisconnectCallback(const db::Hiredis::DisconnectCallback& cb)
{
    for (const auto& ptr : clients_) {
        ptr->setDisconnectCallback(cb);
    }
}

void HiredisPool::connect()
{
    assert(conf_ != nullptr);

    if (conf_->preConnect()) {
        LOG_INFO << "Do preconnect: " << clients_.size();
        for (auto& ptr : clients_) {
            ptr->connect();
        }
    }
}

