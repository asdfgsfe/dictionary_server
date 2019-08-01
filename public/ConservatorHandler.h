#ifndef _CONSERVATOR_HANDLER_H_
#define _CONSERVATOR_HANDLER_H_

#include <memory>
#include <boost/noncopyable.hpp>

namespace Dictionary
{

class std::string;
class ConservatorHandler : boost::noncopyable
{
  friend class Singleton<ConservatorHandler>;
  using Watcher = std::function<void(zhandle_t *zh, 
                                     int type,
                                     int state, 
                                     const char *path,
                                     void *watcherCtx)>;
public:
  ConservatorHandler(const string& hosts);
  ~ConservatorHandler();

  std::unique_ptr<ConservatorFramework> Framework() const { return framework_; }
  void SetChildrenWatcher(Watcher&& cb) { childrenWatcher_ = cb; }
  void SetExistsWatcher(Watcher&& cb){ existsWatcher_ = cb; }
  void SetDataWatcher(Watcher&& cb){ dataWatcher_ = cb; }
  Watcher ChildrenWatcher() const { return childrenWatcher_; }
  Watcher ExistsWatcher() const { return existsWatcher_; }
  Watcher DataWatcher() const { return dataWatcher_; }
  bool Register(const string& path, const string& ip, const string& port); 
private:
  bool started;
  ConservatorFrameworkFactory factory_;
  std::unique_ptr<ConservatorFramework> framework_;
  Watcher childrenWatcher_;
  Watcher existsWatcher_;
  Watcher dataWatcher_;
};

} //namespace Dictionary

#endif //_CONSERVATOR_HANDLER_H_
