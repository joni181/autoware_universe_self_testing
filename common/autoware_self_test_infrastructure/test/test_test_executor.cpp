#include <gtest/gtest.h>

#include <autoware_self_test_types/types.hpp>

#include "autoware_self_test_infrastructure/self_test_registry.hpp"
#include "autoware_self_test_infrastructure/test_run_pipeline/test_executor.hpp"
#include "autoware_self_test_infrastructure/i_tester_component.hpp"

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

std::shared_ptr<types::TestCase> make_case(const std::string & name, bool pass, double elapsed = 0.001)
{
  auto tc = std::make_shared<types::TestCase>();
  tc->name = name;

  // Deliberately do NOT set TestResult.test_case here (executor has a defensive attach).
  tc->test_function = [pass, elapsed]() {
    types::TestResult tr;
    tr.passed = pass;
    tr.elapsed_time_sec = elapsed;
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

TEST(test_executor, executes_all_test_cases_from_all_registered_testers)
{
  auto & registry = infra::SelfTestRegistry::instance();
  RegistryCleanup cleanup(registry);

  const auto id_a = unique_id("executor_component_a");
  const auto id_b = unique_id("executor_component_b");
  cleanup.ids = {id_a, id_b};

  // tester A: 2 cases (pass, fail)
  std::vector<std::shared_ptr<const types::TestCase>> cases_a{
    make_case("a_pass", true),
    make_case("a_fail", false),
  };
  auto tester_a = std::make_shared<DummyTester>(cases_a);
  registry.register_tester(id_a, tester_a);

  // tester B: 1 case (pass)
  std::vector<std::shared_ptr<const types::TestCase>> cases_b{
    make_case("b_pass", true),
  };
  auto tester_b = std::make_shared<DummyTester>(cases_b);
  registry.register_tester(id_b, tester_b);

  infra::TestExecutor executor(registry);
  const auto run_result = executor.execute();

  ASSERT_EQ(run_result.test_results.size(), 3U);
  EXPECT_GE(run_result.elapsed_time_sec, 0.0);

  // Verify executor attached tc pointer defensively when missing
  for (std::size_t i = 0; i < run_result.test_results.size(); ++i) {
    const auto & tr = run_result.test_results[i];
    ASSERT_TRUE(tr.test_case);
    EXPECT_FALSE(tr.test_case->name.empty());
    EXPECT_GE(tr.elapsed_time_sec, 0.0);
  }
}
