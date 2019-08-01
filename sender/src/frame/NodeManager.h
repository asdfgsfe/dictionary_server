#ifndef _NODE_MANAGET_H_
#define _NODE_MANAGET_H_

#include <boost/noncopyable.hpp>

namespace Sender
{
namespace Frame
{
  
class NodeManager : boost::noncopyable
{
  friend class Singleton<NodeManager>;
public:
  shared_ptr<InetAddress> InetAddress(const shared_ptr<Node>& node) const;
  void StorageNode(std::shared_ptr<Node>&& node);
  void RemoveNode(std::shared_ptr<Node>&& node);
  //main 函数调用 TODO 创建节点
  bool Register(const strging& path, const string& ip, const string& port);
  void ExistsWatcher(const string& path);
  void DataWatcher(const string& path);
  void ChildrenWatcher(const string& path);

private:
  NodeManager(const string& zkhosts);
  ~NodeManager() = default;
  void GetIleafNodes(const string& path);

  ConservatorHandler conservator_;
  unordered_map<shared_ptr<Node>, shared_ptr<InetAddress>> nodes_;
};

} //namespace Frame
} //namespace Sender

#endif //_NODE_MANAGET_H_
