#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

namespace Sender
{
namespace Frame
{

class TreeNode : boost::noncopyable
{
public:
  TreeNode(const std::shared_ptr<Node>& val)
    : value_(val),
      children_()
  {

  }
  ~TreeNode() = default;

private:
  std::shared_ptr<Node> value_;
  std::vector<std::shared_ptr<TreeNode>> children_;
};

} //namespace Frame
} //namespace Sender

#endif //_TREE_NODE_H_
