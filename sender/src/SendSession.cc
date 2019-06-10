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

void SendSession::Send(const char* buf, int len)
{
  client_->Send(buf, len); 
}
void SendSession::Recv();
{
  client_->Recv(buffer_);
}

}
