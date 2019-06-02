#include "ClientConfig.h"

ClientConfig::ClientConfig() 
  : config_("../conf/client.conf")
{
  serverIp_ = config_.GetProperty("CLIENT.SERVER_IP");
  string port = config_.GetProperty("CLIENT.PORT");
  port_ = ::atoi(port.c_str());
}

ClientConfig::~ClientConfig()
{
}
