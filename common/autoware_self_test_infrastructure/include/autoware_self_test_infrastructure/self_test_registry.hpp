#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "i_tester_component.hpp"

namespace autoware_self_test_infrastructure
{

//struct that keeps the weak reference to the tester component and (optionally) metadata
struct TesterHandle
{
  std::string component_id;
  std::weak_ptr<ITesterComponent> tester;
  // TODO optional: tags, estimated_cost_ms, requires_exclusive_access, ... ?
};

class SelfTestRegistry
{
public:
  static SelfTestRegistry & instance();

  void register_tester(
    const std::string & component_id,
    const std::shared_ptr<ITesterComponent> & tester);

  void unregister_tester(const std::string & component_id);

  std::vector<TesterHandle> list_testers() const;

  //this function handles the locking logic for the weak pointer and intentionally returns the
  //pointer to the ITesterComponent directly without the metadata in TestHandle
  std::shared_ptr<ITesterComponent> get_tester(const std::string & component_id) const;

private:
  mutable std::mutex mutex_;
  std::unordered_map<std::string, TesterHandle> testers_;
};

}  // namespace autoware_self_test_infrastructure