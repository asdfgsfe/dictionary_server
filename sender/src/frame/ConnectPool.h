#ifndef _CONNECT_POOL_H_
#define _CONNECT_POOL_H_

#include <vector>
#include <momory>
#include <string>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include "DicClient.h"

namespace Sender
{
namespace Frame
{

class ConnectPool : boost::noncopyable
{
public:
  ConnectPool(const string& name = "ConnectPool");
  ~ConnectPool();
  void Start(int size);
  void Stop();
private:
  std::string name_;
  bool running_;
  std::vector<std::unique_ptr<DicClient>> clients_;
  std::vector<std::unique_ptr<Node>> nodes_;
  std::unordered_map<std::unique_ptr<Node>, std::unique_ptr<DicClient>> nodeToClients_; 
};

} //namespace Frame
} //namespace Sender

#endif //_CONNECT_POOL_H_
