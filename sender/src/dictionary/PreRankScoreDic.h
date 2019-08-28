#ifndef _PRE_RANK_SCORE_DIC_H_
#define _PRE_RANK_SCORE_DIC_H_

#include <vector>
#include <string>
#include <muduo/base/Timestamp.h>

namespace Sender
{
namespace Dictionary
{

//fix protobuf message替代自己结构
struct RankScoreData
{
  uint64_t adspaceId = 0;
  uint32_t solutionId = 0;
  double ctr = -1.0;
  double cvr = -1.0;
  uint64_t timestamp = 0;
};

//撸去hdfs数据，将数据格式聚合为protobuf Message格式 通过TransferData接口扔给Session 对象
//这个类不是线程安全的 仅在自己的IO线程中调用
class PreRankScoreDic : boost::noncopyable
{
  friend class Singlenton<PreRankScoreDic>;
public:
  void TransferData();
  //定时器事件到了回调 唯一对外部的接口
  void IsNewDictArrivedAndFetcher();
private:
  PreRankScoreDic();
  ~PreRankScoreDic();
  bool ParseRow(const vector<std::string>& result, RankScoreData& rank);
  void FetcherData();

  std::string name_;
  uint64_t size_;
  std::vector<RankScoreData> datas_;
  uint64_t muduo::Timestamp lastUpdateTimestamp_;
  bool isSuccess_;
  //muduo::net::Timer monitorUpdate_; fix
};
}
private:

};

}
}


#endif //_PRE_RANK_SCORE_DIC_H_
