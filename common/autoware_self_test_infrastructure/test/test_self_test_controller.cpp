#include <gtest/gtest.h>

#include <autoware_self_test_types/types.hpp>

#include "autoware_self_test_infrastructure/i_tester_component.hpp"
#include "autoware_self_test_infrastructure/self_test_controller.hpp"
#include "autoware_self_test_infrastructure/self_test_registry.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
namespace types = autoware_self_test_types;
namespace infra = autoware_self_test_infrastructure;

std::string unique_id(const std::string & prefix)
{
  const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
  return prefix + "_" + std::to_string(static_cast<std::int64_t>(now));
}

struct RegistryCleanup
{
  explicit RegistryCleanup(infra::SelfTestRegistry & r) : registry(r) {}
  ~RegistryCleanup()
  {
    for (const auto & id : ids) {
      registry.unregister_tester(id);
    }
  }
  infra::SelfTestRegistry & registry;
  std::vector<std::string> ids;
};

std::shared_ptr<types::TestCase> make_case(const std::string & name, bool pass)
{
  auto tc = std::make_shared<types::TestCase>();
  tc->name = name;
  tc->test_function = [pass, tc]() {
    types::TestResult tr;
    tr.passed = pass;
    tr.elapsed_time_sec = 0.001;

    // optional: attach here; controller pipeline should work either way
    tr.test_case = tc;
    return tr;
  };
  return tc;
}

class DummyTester final : public infra::ITesterComponent
{
public:
  explicit DummyTester(std::vector<std::shared_ptr<const types::TestCase>> cases)
  : cases_(std::move(cases)) {}

  const std::vector<std::shared_ptr<const types::TestCase>> & get_test_cases() const override
  {
    return cases_;
  }

private:
  std::vector<std::shared_ptr<const types::TestCase>> cases_;
};

}  // namespace

TEST(self_test_controller, runs_pipeline_end_to_end_and_returns_report_json)
{
  auto & registry = infra::SelfTestRegistry::instance();
  RegistryCleanup cleanup(registry);

  const auto id = unique_id("controller_component");
  cleanup.ids = {id};

  std::vector<std::shared_ptr<const types::TestCase>> cases{
    make_case("controller_pass", true),
    make_case("controller_fail", false),
  };

  auto tester = std::make_shared<DummyTester>(cases);
  registry.register_tester(id, tester);

  infra::SelfTestController controller(registry);
  const std::string report = controller.run();

  EXPECT_NE(report.find("\"total_count\":2"), std::string::npos);
  EXPECT_NE(report.find("\"failed_count\":1"), std::string::npos);
  EXPECT_NE(report.find("\"validation_decision\":false"), std::string::npos);
  EXPECT_NE(report.find("\"name\":\"controller_pass\""), std::string::npos);
  EXPECT_NE(report.find("\"name\":\"controller_fail\""), std::string::npos);
}
