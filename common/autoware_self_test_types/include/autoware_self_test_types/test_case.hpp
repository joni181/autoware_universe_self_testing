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

#ifndef AUTOWARE_SELF_TEST_TYPES__TEST_CASE_HPP_
#define AUTOWARE_SELF_TEST_TYPES__TEST_CASE_HPP_

#include <functional>
#include <string>

#include "autoware_self_test_types/test_result.hpp"

namespace autoware_self_test_types
{
struct TestCase
{
  // not implemented here because it is an optional feature
  // std::string bundle{};

  std::function<TestResult()> test_function{};

  // TODO(jlippss): may be needed later
  // std::string name{};
};
}  // namespace autoware_self_test_types

#endif  // AUTOWARE_SELF_TEST_TYPES__TEST_CASE_HPP_
