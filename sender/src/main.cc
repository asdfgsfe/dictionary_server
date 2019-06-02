#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/EventLoopThread.h>

#include <stdio.h>

#include "DicClient.h"
#include "ClientConfig.h"

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << ::getpid() << ", tid = " << CurrentThread::tid();
  ClientConfig conf;
  EventLoop loop;
  InetAddress serverAddr(conf.serverIp(), conf.port());
  DicClient client(&loop, serverAddr);
  client.connect();
  loop.loop();
}

