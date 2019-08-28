#include "PrerankScore.h"
#include "JOutStream.h"
#include "JInstream.h"

PrerankScore::PrerankScore(const SendSession& session)
  : transfer_(std::bind(&PrerankScore::Execute, this, _1))
{

}

void PrerankScore::Execute(SendSession& session)
{
  JOutStream jos;
  uint16_t cmd = CMD_RANK_SCORE;
  jos << cmd;
  size_t lengthPos = jos.Length();
  jos.Skip(2);
  //TODO datas
  //struct RankScoreData
  //{
  //    uint64_t adspaceId = 0;
  //    uint32_t solutionId = 0;
  //    double ctr = -1.0;
  //    double cvr = -1.0;
  //    uint64_t timestamp = 0;//fix
  //};
  //序列化数据 TODO 数据分片
  for (auto data : datas_)
  {
    jos << data.adspaceId << data.solutionId << data.ctr << data.cvr;
    jos.WriteBytes("\n", 1);
  }
  //包头len
  size_t tailPos = jos.Length();
  jos.Reposition(lengthPos);
  jos << static_cast<uint16>(tailPos + 8 - sizeof(RequestHead));
  //包尾
  jos.Reposition(tailPos);
  unsigned char hash[16];
  MD5 md5;
  md5.MD5Make(hash, static_cast<unsigned char const*>(jos.Data()), jos.Length());
  for (int i=0; i<8; ++i)
  {
    hash[i] = hash[i] ^ hash[i+8];
    hash[i] = hash[i] ^ ((cmd >> (i%2)) & 0xff);
  }
  jos.WriteBytes(hash, 8);
  // 发送请求包
  session.Send(jos.Data(), jos.Length());
  //接受应答包 fix异步 接受的目的是为了打点监控流程是否正常
  //需要异步 异步的目的是为了腾出当前线程去发送别的数据 因为所有fetcher都在一个线程池中
  //错误处理交给错误处理线程 或者采用一个回调函数异步就行 因为这里只是将数据发送到sender的队列中
  session.Recv(); //将Recv函数及以后的操作回调异步化
  JInStream jis((const char*)session.GetResponsePack(), 
                session.GetResponsePack()->head.len+sizeof(ResponseHead));
  jis.Skip(4);
  uint16_t errorCode;
  char errorMsg[31];
  jis >> errorCode;
  jis.ReadBytes(errorMsg, 31);
}
