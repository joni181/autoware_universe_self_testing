#include <gtest/gtest.h>

#include <autoware_self_test_types/types.hpp>

#include "autoware_self_test_infrastructure/test_run_pipeline/reporter.hpp"

#include <memory>
#include <string>
#include <vector>

namespace
{
namespace types = autoware_self_test_types;
namespace infra = autoware_self_test_infrastructure;

std::shared_ptr<types::TestCase> make_case(const std::string & name)
{
  auto tc = std::make_shared<types::TestCase>();
  tc->name = name;
  return tc;
}

}  // namespace

TEST(reporter, builds_json_with_expected_fields)
{
  infra::Reporter reporter;

  types::EvaluationResult eval;
  eval.validation_decision = true;
  eval.validation_reason = "All tests passed.";

  types::TestRunResult run;
  run.elapsed_time_sec = 0.123;

  {
    types::TestResult tr;
    tr.test_case = make_case("case_one");
    tr.passed = true;
    tr.elapsed_time_sec = 0.01;
    run.test_results.push_back(tr);
  }
  {
    types::TestResult tr;
    tr.test_case = make_case("case_two");
    tr.passed = true;
    tr.elapsed_time_sec = 0.02;
    run.test_results.push_back(tr);
  }

  const std::string json = reporter.build(eval, run);

  // Very lightweight JSON assertions (no dependency on a JSON library)
  EXPECT_NE(json.find("\"report_id\":\"self_test_report_"), std::string::npos);
  EXPECT_NE(json.find("\"schema_version\":\"1.0\""), std::string::npos);
  EXPECT_NE(json.find("\"elapsed_time_sec\":0.123"), std::string::npos);

  EXPECT_NE(json.find("\"total_count\":2"), std::string::npos);
  EXPECT_NE(json.find("\"passed_count\":2"), std::string::npos);
  EXPECT_NE(json.find("\"failed_count\":0"), std::string::npos);

  EXPECT_NE(json.find("\"validation_decision\":true"), std::string::npos);
  EXPECT_NE(json.find("\"validation_reason\":\"All tests passed.\""), std::string::npos);

  EXPECT_NE(json.find("\"name\":\"case_one\""), std::string::npos);
  EXPECT_NE(json.find("\"name\":\"case_two\""), std::string::npos);
}
