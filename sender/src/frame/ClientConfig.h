#ifndef _CLIENTCONFIG_H_
#define _CLIENTCONFIG_H_

#include "../../../public/SystemConfig.h"
#include "../../../public/Singleton.h"

using namespace PUBLIC;

class ClientConfig
{
  friend class Singleton<ClientConfig>;
public:
  const string& GetServerIp() const
  {
    return serverIp_;
  }

  unsigned short GetPort() const
  {
    return port_;
  }
private:
  ClientConfig();
  ClientConfig(const ClientConfig& rhs);
  ~ClientConfig();

  SystemConfig config_;
  string serverIp_;
  unsigned short port_;
};

#endif //_CLIENTCONFIG_H_
