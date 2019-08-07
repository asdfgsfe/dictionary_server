#ifndef _TRANSFER_NAMEGER_H_
#define _TRANSFER_NAMEGER_H_

#include <unordered_map>
#include "Singleton.h"
namespace Sender
{
class SendSession;
class TransferManager : boost::noncopyable
{
  friend class Singleton<TransferManager>;
public:
  bool DoTransfer(SendSession& session);
private:
  std::unordered_map<uint16_t, Transfer> actions_;
  TransferManager();
};
} //Sender
#endif //_TRANSFER_NAMEGER_H_
