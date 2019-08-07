#include "ConservatorHandler.h"

namespace Dictionary
{
ConservatorHandler::ConservatorHandler(const string& hosts)
  : started(false)
    factory_(),
    framework_(newClient(hosts))
  {
    assert(!started);
    framework_->start();
  }
}

~ConservatorHandler::ConservatorHandler()
{
  assert(started);
  framework_->close();
}

bool ConservatorHandler::Register(const string& path, const string& ip, const string& port)
{
  CheckExistsAndCreatePath(path);
  assert(ZOK == framework_->checkExists()->withWatcher(existsWatcher_, ((void *) framework_.get()))->forPath(path));
  string ileafData = ip + ":" + port;
  return ZOK == framework_->create()->withFlags(ZOO_EPHEMERAL | ZOO_SEQUENCE)->forPath(path + "node:", ileafData.c_str());
}

void CheckExistsAndCreatePath(const string& path)
{
  if (ZOK == framework_->checkExists()->withWatcher(existsWatcher_, ((void*)framework_.get()))->forPath(path))
  {
    return;
  }
  std::vector<std::string> paths;
  boost::split(paths, path, boost::is_any_of("/"));
  string curPath;
  for (const auto& cur : paths)
  {
    curPath += "/" + cur;
    if (ZOK == framework_->checkExists()->withWatcher(existsWatcher_, ((void *) framework_.get()))->forPath(path))
    {
      continue;
    }
    uint32_t times = 5;
    while (times && ZOK != framework_->create()->withFlags(ZOO_EPHEMERAL)->forPath(curPath)) 
    {                                           
      --times;
      ::poll(nullptr, nullptr, 500);
    }
  }
}
