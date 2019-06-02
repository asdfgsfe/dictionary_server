#include "Client.h"

#include "../Public/Socket.h"

#include <sstream>
using namespace std;

Client::Client() : config_("client.conf")
{
	Socket::Startup();
	serverIp_ = config_.GetProperty("CLIENT.SERVER_IP");
	string port = config_.GetProperty("CLIENT.PORT");
	stringstream ss;
	ss<<port;
	ss>>port_;
}

Client::~Client()
{
	Socket::Cleanup();
}
