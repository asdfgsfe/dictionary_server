#ifndef _TANSFER_ACTION_H_
#define _TANSFER_ACTION_H_

#include <functional>

namespace Sender
{
namespace Commander
{
  void DefaultExecute()
  {
    LOG_WARNING << "Transfer not set ExecuteCallback, do default."
  }

class SendSession;
class TransferAction
{
public:
  typedef std::function<void(SendSession&)> ExecuteCallback;
  //setThreadInitCallback(std::bind(&ChatServer::threadInit, this, _1));
  TransferAction(const ExecuteCallback& cb = DefaultExecute)
    : execute_(cb) //fix bug
  {

  }
  void Execute() 
  { 
    assert(execute_);
    execute_();
  }
  void SetexecuteCallback(const ExecuteCallback& cb)
  {
    execute_ = cb;
  }
private:
  ExecuteCallback execute_; 
};

}
}

#endif //_TANSFER_ACTION_H_
