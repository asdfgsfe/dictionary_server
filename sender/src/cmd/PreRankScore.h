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

private:
  TransferAction transfer_;
};

}
}

#endif //_USER_LOGIN_H_
