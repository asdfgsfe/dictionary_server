#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <boost/noncopable.hpp>
#include <muduo/base/ThreadPool.h>
#include <muduo/net/EventLoop.h>

namespace Sender
{
namespace Frame
{
//类似于mutil Reactor的设计 模仿下muduo实现
//这个class 在main函数中调用
class Scanner : boost::noncopyable
{
public:
  Scanner(int taskQueueSize, int numThreads);
  ~Scanner();
  void StartPool();
  void StopPool();
  void InitUpdateTime();
private:
  muduo::net::EventLoop monitorLoop_;
  uint16_t numThreads_;
  uint16_t taskQueueSize_;
  muduo::ThreadPool fetcherPool_; //fix 使用IO线程池 IO线程池好还是计算线程池好
  unordered_map<int16_t, uint64_t> monitorDicUpdateTime_;
};

} //namespace Sender
} //namespace Frame

#endif //_SCANNER_H_
