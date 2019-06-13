#ifndef _NODE_H_
#define _NODE_H_

#include <boost/noncopyable.hpp>

namespace Sender
{
namespace Frame
{
class Node : boost::noncopyable
{
public:
  Node::Node(uint16_t port, const string& ip)
    : port_(port),
      ip_(ip),
      host_(ip_ + ":" + std::to_string(port_))
  {

  }

private:
  uint16_t port_;
  uint32_t numIp_;
  std::string  ip_;
  std::string name_;
  bool isSelf_;
};

}
}

#endif //_NODE_H_
