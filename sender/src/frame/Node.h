#ifndef _NODE_H_
#define _NODE_H_

#include <string>
#include <boost/noncopyable.hpp>

namespace Sender
{
namespace Frame
{
class Node : boost::copyable
{
public:
  Node::Node(string&& name, string& path, string&& ip = "", string&& port = "") //fix
    : name_(name),
      path_(path),
      port_(port),
      ip_(ip),
  {

  }
  ~Node() = default;

  std::string IP() const { return ip_; }
  uint16_t Port() const { return port_; }
  std::string Name() const { return name_; }
  std::string ZkPath() const { return path_; }
private:
  std::string ip_;
  std::string  port_;
  std::string name_; //"sender" "receiver"
  std::string path_;// path=/dict/sender/node:xxx
};

}
}

#endif //_NODE_H_
