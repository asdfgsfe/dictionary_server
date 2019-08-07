#include <sstream>
#include <fstream>
#include <sys/stat.h>

#include <boost/algorithm/string.hpp>
#include <muduo/base/Logging.h>

#include "PreRankScoreDic.h"


namespace Sender
{
namespace Dictionary
{
PreRankScoreDic::PreRankScoreDic()
  : name_("../data/seat_rankscore.txt"),
    size_(0),
    text_(),
    lastUpdateTimestamp_(0)
    isSuccess_(false)
{
}

~PreRankScoreDic::PreRankScoreDic()
{
}

void PreRankScoreDic::FetcherData()
{
  ifstream is;
  ofstream os;
  is.exceptions(std::ios::failbit | std::ios::badbit);
  os.exceptions(std::ios::failbit | std::ios::badbit);
  if (access(file_name.c_str(), F_OK|R_OK) < 0)
  {
    LOG_ERROR << "rank_file[" << "] not exist or not readable!";
    return;
  }
  
  //TODO fetcher from hdfs
  try 
  {
    is.open(file_name, ios::in);
    //TODO monitor
    //FetcherCommon::MonitoringFetcherStatus(name_, is_success_, update_type_);
    std::vector<RankScoreData> datas;
    string line;
    while(getline(is, line))
    {
      if (is.eof())
      {
        break;
      }
      vector<string> result;
      boost::Split(result, line, boost::is_any_of("\001"));
      RankScoreData rankScore;
      if (!ParseRow(result, rank))
      {
        LOG_ERROR << "ParseBannerRow error, skip, with line=" << line;
        continue;
      }
      datas.emplace_back(std::move(rankScore));
      line = "";
    }
    datas_.swap(datas);
    is.close();
  }
  catch(std::system_error& e)
  {
    if (!is.eof())
    {
      LOG_ERROR << "GetBannerDataFromFile catch system_error[" << strerror(errno) << "]";
      return;
    }
  }
  catch(std::ios_base::failure &e)
  {
    if (!is.eof())
    {
      LOG_ERROR << "GetBannerDataFromFile catch ifstream failure[" << strerror(errno) << "]";
      return;
    }
  }
  catch(exception &e)
  {
    if (!is.eof())
    {
      LOG_ERROR << "GetBannerDataFromFile catch exception[" << strerror(errno) << "]";
      return;
    }
  }
  isSuccess = true;
  //FetcherCommon::MonitoringDataSize(name_, datas.size());
}

bool PreRankScoreDic::ParseRow(const vector<string>& result, RankScoreData& rank)
{
  if (result.size() != 5)
  {
    return false;
  }
  stringstream ss;
  ss << result[0];
  ss >> rank.adspaceId;
  ss.str("");
  ss.clear();

  ss << result[1];
  ss >> rank.solutionId;
  ss.str("");
  ss.clear();

  ss << result[2];
  ss >> rank.ctr;
  ss.str("");
  ss.clear();

  ss << result[3];
  ss >> rank.cvr;
  ss.str("");
  ss.clear();

  ss << result[4];
  ss >> rank.timestamp;
  ss.str("");
  ss.clear();
  return true;
}

void PreRankScoreDic::TransferData()
{
  if (datas_.empty() || !isSuccess_)
  {
    return;
  }
  Sendsession session(CMD_RANK_SCORE);
  Singleton<TransferManager>::Instance().DoTransfer(bs);
  
  //fix kafka打点 全局信息
  //PipelineInfo info;
  if (session.ErrorCode == 0)
  {
    info.status = 0;
    info.msg = "ok";
    info.RecvSize = session.Size();
    info.endTime = muduo::Timestamp();
  }
  else
  {
    info.status = ErrorCode;
    info.msg = "-ERROR-";
    info.errorText = session.errorMsg;
    //TODO 报警
  }
}

void PreRankScoreDic::IsNewDictArrivedAndFetcher()
{
  if (access(name.c_str(), F_OK|R_OK) < 0)
  {
    return;
  }
  struct stat buf;
  if (stat(rank_file.c_str(), &buf) < 0)
  {
    char msg[256];
    strerror_r(errno, msg, 256);
    LOG_ERROR << "dictionary name[" << name_ << "stat error[" << msg << "]";
    return;
  }
  time_t current_update_time = buf.st_ctime;
  if (current_update_time <= lastUpdateTimestamp_)
  {
    return;
  }
  lastUpdateTimestamp_ = current_update_time;
  //fix 异步回调 这两个操作
  FetcherData();
  TransferData();
}

} //namespace Dictionary
} //namespace Sender

