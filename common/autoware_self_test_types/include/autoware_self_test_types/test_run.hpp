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

#ifndef AUTOWARE_SELF_TEST_TYPES__TEST_RUN_HPP_
#define AUTOWARE_SELF_TEST_TYPES__TEST_RUN_HPP_

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace autoware_self_test_types
{
struct TestCase;  // forward declaration to fix circular dependency

struct TestRun
{
  // TODO(jlippss): do we really need this?
  // std::string test_run_id{};

  // time recorded when run is started
  std::chrono::system_clock::time_point start_time{};

  // shared_ptr avoids copying and eeps ownership in the component registry
  std::vector<std::shared_ptr<const TestCase>> test_cases{};
};
}  // namespace autoware_self_test_types

#endif  // AUTOWARE_SELF_TEST_TYPES__TEST_RUN_HPP_
