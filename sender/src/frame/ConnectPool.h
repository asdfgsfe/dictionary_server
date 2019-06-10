#ifndef _CONNECT_POOL_H_
#define _CONNECT_POOL_H_

#include <vector>
#include <momory>
#include <string>
#include <boost/noncopyable.hpp>
#include "DicClient.h"

namespace Sender
{
namespace Frame
{

class ConnectPool : boost::noncopyable
{
public:
  ConnectPool(const string& name = "ConnectPool", uint16_t capacity = 10);
  ~ConnectPool();
  void Start(int size);
  void Stop();
private:
  std::string name_;
  const uint16_t maxClientsNum_;
  bool running_;
  std::vector<std::unique_ptr<DicClient>> clients_;
  std::vector<std::unique_ptr<Node>> nodes_;
};

} //namespace Frame
} //namespace Sender

#endif //_CONNECT_POOL_H_
