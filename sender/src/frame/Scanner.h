#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <boost/noncopable.hpp>
#include <muduo/base/ThreadPool.h>
#include <muduo/net/EventLoop.h>

namespace Sender
{
namespace Frame
{
class Scanner : boost::noncopyable
{
public:
  Scanner(int taskQueueSize, int numThreads);
  ~Scanner();
private:
  muduo::net::EventLoop monitorLoop_;
  uint16_t numThreads_;
  uint16_t taskQueueSize_;
  muduo::ThreadPool fetcherPool_;
  unordered_map<int16_t, uint64_t> monitorDicUpdateTime_;
};

} //namespace Sender
} //namespace Frame

#endif //_SCANNER_H_
