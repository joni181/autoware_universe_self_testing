#include <gtest/gtest.h>

#include <autoware_self_test_types/types.hpp>

#include "autoware_self_test_infrastructure/test_run_pipeline/evaluator.hpp"

#include <string>
#include <vector>

namespace
{
namespace types = autoware_self_test_types;
namespace infra = autoware_self_test_infrastructure;

types::TestRunResult make_run_result(std::initializer_list<bool> passes)
{
  types::TestRunResult r;
  for (bool p : passes) {
    types::TestResult tr;
    tr.passed = p;
    r.test_results.push_back(tr);
  }
  r.elapsed_time_sec = 0.01;
  return r;
}
}  // namespace

TEST(evaluator, returns_false_when_no_tests_executed)
{
  infra::Evaluator evaluator;
  types::TestRunResult empty;
  const auto eval = evaluator.evaluate(empty);

  EXPECT_FALSE(eval.validation_decision);
  EXPECT_FALSE(eval.validation_reason.empty());
}

TEST(evaluator, returns_false_when_any_test_failed)
{
  infra::Evaluator evaluator;
  const auto run_result = make_run_result({true, false, true});
  const auto eval = evaluator.evaluate(run_result);

  EXPECT_FALSE(eval.validation_decision);
  EXPECT_FALSE(eval.validation_reason.empty());
}

TEST(evaluator, returns_true_when_all_tests_passed)
{
  infra::Evaluator evaluator;
  const auto run_result = make_run_result({true, true, true});
  const auto eval = evaluator.evaluate(run_result);

  EXPECT_TRUE(eval.validation_decision);
  EXPECT_FALSE(eval.validation_reason.empty());
}
