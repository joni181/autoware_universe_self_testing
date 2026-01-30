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

#include <gtest/gtest.h>

#include <autoware_self_test_types/types.hpp>

using autoware_self_test_types::EvaluationResult;
using autoware_self_test_types::TestCase;
using autoware_self_test_types::TestResult;

TEST(self_test_types, evaluation_result_defaults)
{
  EvaluationResult r;
  EXPECT_FALSE(r.validation_decision);
  EXPECT_TRUE(r.validation_reason.empty());
  EXPECT_DOUBLE_EQ(r.adequacy, 0.0);
  EXPECT_DOUBLE_EQ(r.coverage, 0.0);
}

TEST(self_test_types, test_case_invokable_smoke)
{
  auto tc = std::make_shared<TestCase>();
  tc->test_function = []() {
      TestResult tr;
      tr.passed = true;
      return tr;
    };

  auto tr = tc->test_function();
  EXPECT_TRUE(tr.passed);
}

TEST(self_test_types, test_result_can_reference_test_case)
{
  auto tc = std::make_shared<TestCase>();
  TestResult tr;
  tr.test_case = tc;
  EXPECT_EQ(tr.test_case, tc);
}
