#include <muduo/base/Logging.h>

#include "TransferManager.h"
#include "SendSession.h"

namespace Sender
{
TransferManager::TransferManager()
{
  //TODO 从配置文件获取各种词典
  m_actions[CMD_LOGIN] = new UserLogin;
  m_actions[CMD_OPEN_ACCOUNT] = new OpenAccount;
  m_actions[CMD_DEPOSIT] = new Deposit;
  m_actions[CMD_WITHDRAW] = new Withdrawal;
  m_actions[CMD_TRANSFER] = new Transfer;
  m_actions[CMD_BALANCE_INQUIRY] = new BalanceInquiry;
  m_actions[CMD_CHANGE_PASSWORD] = new ChangePassword;
  m_actions[CMD_DAY_BILL] = new QueryDayBill;
  m_actions[CMD_HISTORY_BILL] = new QueryHistoryBill;
  m_actions[CMD_ACCOUNT_HISTORY_BILL] = new QueryAccountHistoryBill;
  m_actions[CMD_CLOSE_ACCOUNT] = new CloseAccount;
}
~TransferManager::TransferManager()
{
  
}

bool TransferManager::DoTransfer(SendSession& session)
{
  uint16_t cmd = session.Commander();
  auto transfer = actions_.find(cmd);
  if (transfer != actions.end())
  {
    transfer->second.Execute(session);    
    return true;
  }
  LOG_INFO << "DoTransfer cmd=" << cmd << " not found.";
  return false;
}

}//Sender
