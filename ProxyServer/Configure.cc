#include "Configure.h"
#include <string>

#define RESERVED_FDS 32
#define LISTEN_PORT (7070)

Configure::Configure()
    : preConnection_(true),
      listenAddr_(LISTEN_PORT)
{

}

Configure::Configure(std::string confFile)
    : confFile_(confFile),
      preConnection_(true),
      listenAddr_(7070)
{
    root_ = YAML::LoadFile(confFile);
}

void Configure::open(const char *fileName) /*throw exception*/
{
    root_ = YAML::LoadFile(fileName);
}

void Configure::parse()
{
    if (root_.IsNull()) {
        return;
    }

    /// listen address
    YAML::Node ip = root_["listen"];
    YAML::Node port = root_["listen_port"];
    uint16_t nport = LISTEN_PORT;
    std::string ipStr("0.0.0.0");
    if (port.IsDefined()) {
        nport = port.as<uint16_t>();
    }

    if (ip.IsDefined()) {
        ipStr = ip.as<std::string>();
    }
    muduo::net::InetAddress listenAddr(ipStr, nport);
    listenAddr_ = listenAddr;

    /// redis server address
    YAML::Node addr = root_["redis_servers"];
    if (addr.IsDefined()) {
        for (YAML::Node::iterator it = addr.begin(); it != addr.end(); ++it) {
            serverAddrs_.emplace_back((*it)["ip"].as<std::string>(), (*it)["port"].as<uint16_t>());
            LOG_INFO << "redis address:"
                     << (*it)["ip"].as<std::string>()
                     << ":" << (*it)["port"].as<uint16_t>();
        }
    } else {
        LOG_FATAL << "Please configure redis address!";
    }

    /// pre connection or not ?
    YAML::Node preConnection = root_["preconnection"];
    if (preConnection.IsDefined()) {
        preConnection_ = root_["preconnection"].as<bool>();
    }
}

void Configure::calcConnections()
{
    //status = getrlimit(RLIMIT_NOFILE, &limit);
    //ctx->max_nfd = (uint32_t)limit.rlim_cur;
    // client = maxfd - server - reserved.
    // ctx->max_ncconn = ctx->max_nfd - ctx->max_nsconn - RESERVED_FDS;
}


