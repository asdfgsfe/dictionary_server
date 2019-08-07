#include "DicClient.h"
#include "SendSession.h"

namespace Sender
{
SendSession::SendSession(uint16_t cmd)
  : client(new DicClient()),
    name_(std::to_string(cmd)),
    cmd_(cmd)
    buffer_()
  {
    client_.connect();
  }

~SendSession::SendSession()
{
  client_.disconnect();
}

void SendSession::SendData(muduo::net::Buffer&& data)
{
  //client_->Send(buf, len); 
  connManager_->PutData(std::foward(data));
}

//TODO fix 异步接受
void SendSession::Recv();
{
  client_->Recv(buffer_);
}

}
