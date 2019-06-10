#ifndef _NODE_MANAGER_H_
#define _NODE_MANAGER_H_

#include <memory>
#include <boost/noncopyable.hpp>

namespace Dictionary
{

class std::string;
class NodeManager : boost::noncopyable
{
  friend class Singleton<NodeManager>;
public:
  std::unique_ptr<ConservatorFramework> FrameworkPtr() const
  {
    return framework_;
  }
private:
  NodeManager(const string& hosts)
    : started(false),
      factory_(),
      framework_(new factory.newClient(hosts))
  {
    assert(framework_ && !started);
    framework_->start();
  }

  ~NodeManager()
  {
    assert(framework_ && started);
    framework_->close();
  }

  bool started;
  ConservatorFrameworkFactory factory_;
  std::unique_ptr<ConservatorFramework> framework_;
};

} //namespace Dictionary

#endif //_NODE_MANAGER_H_
