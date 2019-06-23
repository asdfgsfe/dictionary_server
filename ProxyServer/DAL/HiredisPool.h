#ifndef HIREDISPOOL_H
#define HIREDISPOOL_H
#include "Hiredis.h"
#include <boost/noncopyable.hpp>
#include <vector>
#include <mutex>
#include <deque>
#include <queue>

#include "HashKit/HashKit.h"
#include "Configure.h"

void connectCallback(db::Hiredis* c, int status);
void disconnectCallback(db::Hiredis* c, int status);

namespace db {

typedef std::shared_ptr<db::Hiredis> HiredisPtr;

class HiredisPool : muduo::noncopyable
{
public:
    HiredisPool(muduo::net::EventLoop* loop, Configure* conf);

    void setConnectCallback(const db::Hiredis::ConnectCallback& cb);
    void setDisconnectCallback(const db::Hiredis::DisconnectCallback& cb);

    void connect();

    HiredisPtr getHiredis(const char *key, size_t keyLen)// const
    {
        LOG_INFO << "Selected redis server: " << hashFun_(key, keyLen) % poolSize_;
        return clients_[hashFun_(key, keyLen) % poolSize_];
    }

    int getPoolSize() const
    {
        return poolSize_;
    }

private:
    muduo::net::EventLoop* loop_;
    Configure* conf_;
    muduo::net::InetAddress masterAddr_;
    std::vector<muduo::net::InetAddress> serverAddrs_;
    std::vector<HiredisPtr> clients_;
    hasher hashFun_;
    int poolSize_;
    bool closed_;
};
}
#endif
