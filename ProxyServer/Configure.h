#ifndef CONFIGURE_H
#define CONFIGURE_H
#include <muduo/base/Logging.h>
#include "yaml-cpp/yaml.h"
#include <muduo/net/InetAddress.h>
#include <vector>

class Configure : muduo::copyable
{
public:
    Configure();
    explicit Configure(std::string confFile);
    ~Configure() = default;
    void open(const char* fileName);
    void parse();

    bool preConnect() const
    {
        return preConnection_;
    }

    muduo::net::InetAddress getListenAddr() const
    {
        return listenAddr_;
    }

    std::vector<muduo::net::InetAddress> getServerAddrs() const
    {
        return serverAddrs_;
    }

    muduo::Logger::LogLevel getLogLevel() const
    {
        return (muduo::Logger::LogLevel)logLevel_;
    }

    int getPerServerConnNum() const
    {
        return nconn_;
    }

private:
    void calcConnections();
    std::string confFile_;
    YAML::Node root_;
    int logLevel_ = muduo::Logger::DEBUG;
    bool preConnection_;
    int nconn_ = 1;
    muduo::net::InetAddress listenAddr_;
    std::vector<muduo::net::InetAddress> serverAddrs_;
};

#endif // CONFIGURE_H
