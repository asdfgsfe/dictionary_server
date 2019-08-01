#include "NodeManager.h"

namespace Sender
{
namespace Frame
{
NodeManager::NodeManager(const string& zkhosts)
  : conservator_(zkhosts),
{
  conservator_.SetChildrenWatcherCallback(
      std::bind(&NodeManager::ChildrenWatcher, this, _1, _2, _3, _4));
  conservator_.SetExistsWatcherCallback(
      std::bind(&NodeManager::ExistsWatcher, this, _1, _2, _3, _4));
  conservator_.SetDataWatcherCallback(
      std::bind(&NodeManager::DataWatcher, this, _1, _2, _3, _4));
}

//TODO 实际的逻辑放到 conservatorHandler中
bool NodeManager::Register(const string& path, const string& ip, const string& port)
{
  //NodeManager.Register() 在main函数中
  return conservator_.Register(path, ip, port); 
}

shared_ptr<InetAddress> NodeManager::InetAddress(const shared_ptr<Node>& node) const
{
  if (!node || !nodeToAdds_.count(node))
  {
    return nullptr;
  }
  return nodes_.find(node)->second;
}

//坏处一个节点改变 需要获取所有的节点 所有应该监视 /dict/sender目录 和 /dict目录
//sender节点为领事 node-xxx为领事有序节点
void NodeManager::StorageNode(std::shared_ptr<Node>&& node)
{
  assert(node);
  nodes_.emplace(std::move(node), std::make_shared<InetAddress>(node->IP(), node->Port));
}


//TODO 貌似没有用 因为每次都会从zk中全量获取节点一次
void NodeManager::RemoveNode(std::shared_ptr<Node>&& node)
{
  if (!nodes_.count(node))
  {
    nodes_.erase(node);
  }
}

//监视路径下的节点是否存在
void NodeManager::ExistsWatcher(const string& path)
{
  //TODO get create path
  //cout << "exists watcher function called" << endl;
  auto& framework = conservator_->Framework();
  auto& existsWatcher = conservator_->ExistsWatcher();
  auto ret = framework->checkExists()->withWatcher(existsWatcher, 
              static_cast<void*>(framework.get())->forPath(path);
  if (ZNONODE == ret)
  {
    //framework->create()->forPath(path);
    //CheckExistsAndCreatePath(path);
    LOG_WARN << "path[" << path << "] not exits."
  }
  //TODO 路径存在的事情 貌似没有事情可以做

  // reset the watcher
  framework->checkExists()->withWatcher(existsWatcher, framework)->forPath(path);
}

//本质是为了监视节点的数据是否改变 不一定用到
void NodeManager::DataWatcher(const string& path)
{
  //TODO get data
  //cout << "get watcher function called" << endl;
  nodes_.clear();
  GetIleafNodes(path); //path=/dict

  // reset the watch
  auto& framework = conservator_->Framework();
  const auto& childrenWatcher = conservator_->ChildrenWatcher();
  if(framework->checkExists()->forPath(path)) 
  {
    framework->getData()->withWatcher(dataWatcher, framework)->forPath(path);
  }
}

void NodeManager::ChildrenWatcher(const string& path)
{
  //TODO get data
  //cout << "get child watcher function called path=" << path << endl;
  nodes_.clear(); 
  GetIleafNodes(path);
  
  //树形存储
  //root_ = ChildrenNodes(path)
  //reset the watch
  auto& framework = conservator_->Framework();
  const auto& childrenWatcher = conservator_->ChildrenWatcher();
  framework->getChildren()->withWatcher(childrenWatcher, &framework)->forPath(path);
}

//TODO generator Tree storage node
std::shared_ptr<TreeNode> NodeManager::ChildrenNodes(const string& path)
{
  auto& framework = conservator_->Framework();
  const auto& childrenWatcher = conservator_->ChildrenWatcher();
  string data;
  data = framework->getData()->withWatcher(dataWatcher,//fix 
  vector<string> children;
  children = framework->getChildren()->withWatcher(childrenWatcher, &framework)->forPath(path);
                                              ((void*)framework.get()))->forPath(path);
  if (children.empty()) //叶子节点
  {
    vector<strin> info;
    boost::split(data, info, boost::is_any_of(":"));
    assert(info.size() >= 2);
    //info[0] = ip, info[1] = port info[...] = fathers
    //TODO 插入senderHandler tsd中 此处应该回调onConnection函数 实现接口统一
    return std::make_shared<TreeNode>(Node("ileaf", path, info[0], info[1]));
  }
  //not ileaf node to get ileaf node
  vector<string> curNode;
  boost::split(path, curNode, boost::is_any_of("/"));
  assert(!curNode.empty());
  std::shared_ptr<TreeNode> cur = std::make_shared<TreeNode>(Node(curNode.back(), path)));
  for (const auto& child : children)
  {
    string child = path + "/" + child;
    cur->emplace_back(ChildrenNodes(child));
  }
  return cur;
}

void NodeManager::GetIleafNodes(const string& path)
{
  auto& framework = conservator_->Framework();
  const auto& childrenWatcher = conservator_->ChildrenWatcher();
  string data;
  data = framework->getData()->withWatcher(dataWatcher, 
  vector<string> children;
  children = framework->getChildren()->withWatcher(childrenWatcher, &framework)->forPath(path);
                                              ((void*)framework.get()))->forPath(path);
  if (children.empty()) //叶子节点
  {
    vector<strin> info = boost::split(data, info, boost::is_any_of(":"));
    assert(info.size() >= 2);
    //info[0] = ip, info[1] = port info[...] = fathers
    sharted_ptr<Node>node = make_shared(info[0], info[1], path); //fix node具有移语义
    StorageNode(std::move(node));
    //TODO 插入senderHandler tsd中 此处应该回调onConnection函数 实现接口统一
    return;
  }
  //not ileaf node to get ileaf node
  for (const auto& child : children)
  {
    string child = path + "/" + child;
    GetIleafNodes(child);
  }
}

} //namespace Frame
} //namespace Sender
