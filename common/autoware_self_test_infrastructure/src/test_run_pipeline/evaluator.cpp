// Copyright 2026 Jonathan Lippss
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "autoware_self_test_infrastructure/test_run_pipeline/evaluator.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace autoware_self_test_infrastructure
{
namespace types = autoware_self_test_types;

Evaluator::Evaluator(const ValidationPolicy & policy)
: policy_(policy)
{
}

void Evaluator::set_validation_policy(const ValidationPolicy & policy)
{
  policy_ = policy;
}

const ValidationPolicy & Evaluator::get_validation_policy() const
{
  return policy_;
}

types::EvaluationResult Evaluator::evaluate(const types::TestRunResult & test_run_result) const
{
  types::EvaluationResult out{};

  out.failed_test_count = std::count_if(
    test_run_result.test_results.begin(), test_run_result.test_results.end(),
    [](const auto & tr) { return tr.result == types::TestResultStatus::FAIL; });

  const auto [decision, reason] = validate_(test_run_result);

  out.validation_decision = decision;
  out.validation_reason = reason;

  return out;
}

std::pair<bool, std::string> Evaluator::validate_(const types::TestRunResult & test_run_result) const
{
  if (test_run_result.test_results.empty()) {
    return {false, "No tests have been executed."};
  }

  std::size_t pass_count = 0;
  std::size_t skip_count = 0;
  std::size_t failed_count = 0;
  for (const auto & tr : test_run_result.test_results) {
    switch (tr.result) {
      case types::TestResultStatus::PASS:
        ++pass_count;
        break;
      case types::TestResultStatus::SKIP:
        ++skip_count;
        break;
      case types::TestResultStatus::FAIL:
      default:
        ++failed_count;
        break;
    }
  }

  if (failed_count > 0U) {
    std::ostringstream oss;
    oss << failed_count << " tests failed.";
    return {false, oss.str()};
  }

  if (skip_count > 0U) {
    std::ostringstream oss;
    oss << pass_count << " tests passed, " << skip_count << " tests skipped.";
    return {true, oss.str()};
  }

  return {true, "All tests passed."};
}

}  // namespace autoware_self_test_infrastructure
