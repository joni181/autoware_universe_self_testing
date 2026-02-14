#include <gtest/gtest.h>

#include <autoware_self_test_infrastructure/i_tester_component.hpp>
#include <autoware_self_test_infrastructure/self_test_registry.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace infra = autoware_self_test_infrastructure;

class DummyTester : public infra::ITesterComponent
{
public:
  const std::vector<std::shared_ptr<const autoware_self_test_types::TestCase>> & get_test_cases() const override
  {
    return cases_;
  }

private:
  std::vector<std::shared_ptr<const autoware_self_test_types::TestCase>> cases_{};
};

namespace
{
std::string unique_id(const std::string & prefix)
{
  const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
  return prefix + "_" + std::to_string(static_cast<std::int64_t>(now));
}
}  // namespace

TEST(self_test_registry, register_list_get_tester_unregister)
{
  auto & reg = infra::SelfTestRegistry::instance();

  const std::string component_id = unique_id("detection_area_registry_test");
  auto tester = std::make_shared<DummyTester>();

  reg.register_tester(component_id, tester);

  // list contains it
  const auto handles = reg.list_testers();
  ASSERT_FALSE(handles.empty());

  bool found = false;
  for (const auto & h : handles) {
    if (h.component_id == component_id) {
      found = true;
      break;
    }
  }
  EXPECT_TRUE(found);

  // get_tester returns a live shared_ptr while tester is still alive
  {
    auto live = reg.get_tester(component_id);
    ASSERT_TRUE(live);
  }

  // Now release the last owning reference; registry only holds weak_ptr.
  tester.reset();

  // weak_ptr should now be expired => get_tester returns empty shared_ptr
  auto expired = reg.get_tester(component_id);
  EXPECT_FALSE(expired);

  // cleanup: remove entry
  reg.unregister_tester(component_id);

  // after unregister, lookup should still return empty
  auto after_unreg = reg.get_tester(component_id);
  EXPECT_FALSE(after_unreg);
}
