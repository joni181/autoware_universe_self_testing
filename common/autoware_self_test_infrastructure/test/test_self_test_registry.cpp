#include <gtest/gtest.h>

#include <autoware_self_test_infrastructure/self_test_registry.hpp>
#include <autoware_self_test_infrastructure/i_tester_component.hpp>

#include <memory>

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

TEST(self_test_registry, register_list_try_get_unregister)
{
  auto & reg = infra::SelfTestRegistry::instance();

  const std::string module_id = "detection_area_registry_test";
  auto tester = std::make_shared<DummyTester>();

  reg.register_tester(module_id, tester);

  // list contains it
  const auto handles = reg.list_testers();
  ASSERT_FALSE(handles.empty());

  // Acquire a live shared_ptr, then release it before checking expiration.
  {
    auto live = reg.try_get_tester(module_id);
    ASSERT_TRUE(live);
  }  // live destroyed here

  // Now release the last owning reference.
  tester.reset();

  // weak_ptr should now be expired => lock() returns empty shared_ptr
  auto expired = reg.try_get_tester(module_id);
  EXPECT_FALSE(expired);

  // cleanup (registry entry may still exist as stale metadata)
  reg.unregister_tester(module_id);
  EXPECT_FALSE(reg.get_tester(module_id).has_value());
}
