#include "autoware_self_test_infrastructure/self_test_registry.hpp"

#include <utility>

namespace autoware_self_test_infrastructure
{

// singleton
SelfTestRegistry & SelfTestRegistry::instance()
{
  static SelfTestRegistry inst;
  return inst;
}

void SelfTestRegistry::register_tester(
  const std::string & component_id,
  const std::shared_ptr<ITesterComponent> & tester)
{
  std::lock_guard<std::mutex> lock(mutex_);

  TesterHandle handle;
  handle.component_id = component_id;
  handle.tester = tester;

  testers_[component_id] = std::move(handle);
}

void SelfTestRegistry::unregister_tester(const std::string & component_id)
{
  std::lock_guard<std::mutex> lock(mutex_);
  testers_.erase(component_id);
}

std::vector<TesterHandle> SelfTestRegistry::list_testers() const
{
  std::lock_guard<std::mutex> lock(mutex_);

  std::vector<TesterHandle> out;
  out.reserve(testers_.size());

  for(const auto & kv : testers_) {
    out.push_back(kv.second);
  }

  return out;
}

std::shared_ptr<ITesterComponent> SelfTestRegistry::get_tester(const std::string & component_id) const
{
  std::lock_guard<std::mutex> lock(mutex_);

  const auto it = testers_.find(component_id);
  if(it == testers_.end()){
    return {};
  }

  // convert weak_ptr -> shared_ptr (may be empty if the tester expired)
  return it->second.tester.lock();
}

} // namespace autoware_self_test_infrastructure
