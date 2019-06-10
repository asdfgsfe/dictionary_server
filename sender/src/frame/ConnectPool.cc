#inclue "ConnectPool.h"

namespace Sender
{
namespace Frame
{
ConnectPool::ConnectPool(const string& name, uint16_t capacity)
  : name_(name),
    maxClientsNum_(capacity),
    running_(false),
    clients_()
{
}

ConnectPool::~ConnectPool()
{
  if (running_) { Stop(); }
}

void ConnectPool::Start(int size)
{
  size = size < maxClientsNum_ ? size : maxClientsNum_;
  assert(clients_.empty());
  for (int i = 0; i < size; ++i)
  {
     
  }
}

} //namespace Sender
} //namespace Frame
