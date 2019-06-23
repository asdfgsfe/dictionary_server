#include <google/protobuf/stubs/common.h>
#include <muduo/base/Singleton.h>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include "yaml-cpp/yaml.h"

#include "Configure.h"
#include "ProxyServer.h"
#include "DAL/Hiredis.h"
#include "DAL/HiredisPool.h"

#include <fstream>
#include <list>
#include <string>
#include <thread>
#include <functional>

#include "Message/Query.pb.h"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <configure file>\n", argv[0]);
    } else {
        Configure conf(argv[1]);
        conf.parse();
        // TODO: Create PID file.
        muduo::Logger::setLogLevel(conf.getLogLevel());
        LOG_INFO << "Redis Proxy Server is Running. PID = " << getpid();

        muduo::net::EventLoop loop;
        ProxyServer server(&loop, &conf, 1); // single thread
        server.start();
        loop.loop();
    }
    return 0;
}
