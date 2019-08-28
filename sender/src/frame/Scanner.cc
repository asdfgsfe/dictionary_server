#include "Scanner.h"
#include "PreRankScoreDic.h"

Scanner::Scanner(int taskQueueSize, int numThreads)
  : monitorLoop_(),
    numThreads_(numThreads),
    taskQueueSize_(taskQueueSize),
    fetcherPool_("fetcherDictPool"),
{
  loop.loop();
}

~Scanner::Scanner()
{
  loop.quit();
}

void Scanner::StartPool()
{
  fetcherPool_.setMaxQueueSize(taskQueueSize_);
  fetcherPool_.start(numThreads_);
}

void Scanner::StopPool()
{
  fetcherPool_.stop();
}

void Scanner::InitUpdateTime()
{
  //TODO 读取配置文件各词典名字及定时器检测跨度
  //工场模式 在这进行词典对象的创建?
  //<k,v> <cmd, DictObject> DictObject每个词典及属性
  monitorDicUpdateTime_[CMD_RANK_SCORE] = 3;
}

void Scanner::RegisterMonitor()
{
  if (monitorDicUpdateTime_.empty())
  {
    return;
  }
  //仅仅只是将各种定时器注册到了一个EventLoop中 在一个单线程中运行
  //fix multi Reactor模式
  for (auto timer : monitorDicUpdateTime_)
  {
    monitorLoop_.runEvery(timer.second, 
      std::bind(&Scanner::IsNewDictArrived, &Scanner, timer.first));
  }
}

//IsNewDictArrived函数也是类似于多态的基类 
//这里如何去掉switch case 做成基于对象的静态多态呢 功能上仅仅只是包装了每个词典的IsNewDictArrived
void Scanner::IsNewDictArrived(uint16_t cmd)
{
  switch (cmd)
  {
    case CMD_RANK_SCORE:
      fetcherPool_.run(std::bind(&PreRankScoreDic::IsNewDictArrived, &PreRankScoreDic));
      break;
    default:
      break;
  }
}

