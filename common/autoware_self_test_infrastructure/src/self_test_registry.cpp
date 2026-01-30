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
    const std::string & module_id,
    const std::shared_ptr<ITesterComponent> & tester)
{
    std::lock_guard<std::mutex> lock(mutex_);

    TesterHandle handle;
    handle.module_id = module_id;
    handle.tester = tester;

    testers_[module_id] = std::move(handle);
}

void SelfTestRegistry::unregister_tester(const std::string & module_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    testers_.erase(module_id);
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

std::optional<TesterHandle> SelfTestRegistry::get_tester(const std::string & module_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = testers_.find(module_id);
    if(it == testers_.end()) {
        return std::nullopt;
    }

    return it->second;
}

std::shared_ptr<ITesterComponent> SelfTestRegistry::try_get_tester(const std::string & module_id) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = testers_.find(module_id);
    if(it == testers_.end()){
        return {};
    }

    // convert weak_ptr -> shared_ptr (may be empty if the tester expired)
    return it->second.tester.lock();
}

} // namespace autoware_self_test_infrastructure
