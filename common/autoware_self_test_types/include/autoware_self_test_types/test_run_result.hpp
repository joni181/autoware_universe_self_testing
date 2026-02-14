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

#ifndef AUTOWARE_SELF_TEST_TYPES__TEST_RUN_RESULT_HPP_
#define AUTOWARE_SELF_TEST_TYPES__TEST_RUN_RESULT_HPP_

#include <vector>

#include "autoware_self_test_types/test_result.hpp"
#include "autoware_self_test_types/test_run.hpp"

namespace autoware_self_test_types
{
struct TestRunResult
{
  TestRun test_run{};
  std::vector<TestResult> test_results{};

  // seconds for the whole run (all test cases)
  double elapsed_time_sec{0.0};
};
}  // namespace autoware_self_test_types

#endif  // AUTOWARE_SELF_TEST_TYPES__TEST_RUN_RESULT_HPP_
