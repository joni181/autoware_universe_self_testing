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

#include "autoware_self_test_infrastructure/test_run_pipeline/test_executor.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware_self_test_infrastructure
{
namespace types = autoware_self_test_types;

TestExecutor::TestExecutor(SelfTestRegistry & registry) : registry_(registry)
{
}

types::TestRunResult TestExecutor::execute()
{
  types::TestRunResult run_result;

  const auto start = std::chrono::steady_clock::now();

  // discover registered testers
  const auto handles = registry_.list_testers();

  for (const auto & handle : handles) {
    auto tester = registry_.get_tester(handle.component_id);
    if (!tester) {
        continue;  // tester not alive anymore
    }

    // get test cases for this tester, execute all
    const auto & test_cases = tester->get_test_cases();
    for (const auto & tc_ptr : test_cases) {
      if (!tc_ptr || !static_cast<bool>(tc_ptr->test_function)) {
        continue;
      }
      // execute the test case
      types::TestResult tr = tc_ptr->test_function();

      // defensive: if the callee didn't attach tc pointer
      if (!tr.test_case) {
        tr.test_case = tc_ptr;
      }

      run_result.test_results.push_back(std::move(tr));
      run_result.test_run.test_cases.push_back(tc_ptr);
    }
  }

  const auto end = std::chrono::steady_clock::now();
  run_result.elapsed_time_sec = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

  return run_result;
}
}  // namespace autoware_self_test_infrastructure
