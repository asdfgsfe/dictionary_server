#ifndef _PRE_RANK_SCORE_DIC_H_
#define _PRE_RANK_SCORE_DIC_H_

#include <vector>
#include <string>
#include <muduo/base/Timestamp.h>

namespace Sender
{
namespace Dictionary
{
struct RankScoreData
{
  uint64_t adspaceId = 0;
  uint32_t solutionId = 0;
  double ctr = -1.0;
  double cvr = -1.0;
  uint64_t timestamp = 0;
};


class PreRankScoreDic : boost::noncopyable
{
  friend class Singlenton<PreRankScoreDic>;
public:
  void TransferData();
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
