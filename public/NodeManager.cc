#include "NodeManager.h"

namespace Dictionary
{
NodeManager::NodeManager(const string& hosts)
  : started(false)
    factory_(),
    framework_(newClient(hosts))
  {
    assert(!started);
    framework_->start();
  }
}

~NodeManager::NodeManager()
{
  assert(started);
  framework_->close();
}
