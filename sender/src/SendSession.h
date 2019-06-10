#ifndef _SENDER_SESSION_H_
#define _SENDER_SESSION_H_

#include <memory>
#include <string>

#include <muduo/base/Buffer.h>

namespace Sender
{
class Client;

class SendSession : boost::noncopyable
{
public:
  explicit SendSession(uint16_t cmd);
  ~SendSession();
  void Send(const char* buf, int len);
  void Recv();
  uint16_t Commander() const
  {
    return cmd_;
  }
private:
  std::unique_ptr<DicClient> client_;
  std::string name_;
  uint16_t cmd_;
  muduo::net::Buffer buffer_;
};
} //Sender

#endif //_SENDER_SESSION_H_
