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
  //TODO 读取配置文件各个词典的跟新跨度
  monitorDicUpdateTime_[CMD_RANK_SCORE] = 3;
}

void Scanner::RegisterMonitor()
{
  if (monitorDicUpdateTime_.empty())
  {
    return;
  }
  for (auto timer : monitorDicUpdateTime_)
  {
    monitorLoop_.runEvery(timer.second, 
      std::bind(&Scanner::IsNewDictArrived, &Scanner, timer.first));
  }
}

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

