#ifndef _USER_LOGIN_H_
#define _USER_LOGIN_H_

namespace Sender
{
namespace Commander
{
class PrerankScore
{
public:
  PrerankScore(const SendSession& session);
  void Execute(SendSession& session)
  //SendSession负责fetcher与sender通信，仅三作用1.序列化 2.发送(写队列)3.接受打点及错误处理
  //sender与业务无关 仅仅只是对接muduo 发送网络数据
private:
  TransferAction transfer_;
};

}
}

#endif //_USER_LOGIN_H_
