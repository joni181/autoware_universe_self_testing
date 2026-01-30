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

#ifndef AUTOWARE_SELF_TEST_TYPES__TEST_RESULT_HPP_
#define AUTOWARE_SELF_TEST_TYPES__TEST_RESULT_HPP_

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace autoware_self_test_types
{
struct TestCase;  // forward declaration to fix circular dependency

using error_map_t = std::unordered_map<std::string, std::string>;

struct TestResult
{
  // using shared_ptr avoids copying TestCase (and keeps "ownership" in the component registry)
  std::shared_ptr<const TestCase> test_case{};

  bool passed{false};

  double elapsed_time_sec{0.0};

  // optional: only if !passed
  std::optional<error_map_t> error{};

  double coverage{0.0};
};
}  // namespace autoware_self_test_types

#endif  // AUTOWARE_SELF_TEST_TYPES__TEST_RESULT_HPP_
