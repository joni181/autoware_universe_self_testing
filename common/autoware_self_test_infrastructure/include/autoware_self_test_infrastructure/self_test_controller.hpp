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

#pragma once

#include <autoware_self_test_types/types.hpp>

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "autoware_self_test_infrastructure/test_run_pipeline/evaluator.hpp"
#include "autoware_self_test_infrastructure/test_run_pipeline/reporter.hpp"
#include "autoware_self_test_infrastructure/test_run_pipeline/test_executor.hpp"
#include "autoware_self_test_infrastructure/self_test_registry.hpp"

namespace autoware_self_test_infrastructure
{

class SelfTestController
{
public:
  explicit SelfTestController(SelfTestRegistry & registry);

  // runs all test cases and returns the final JSON report.
  std::string run();

private:
  TestExecutor executor_;
  Evaluator evaluator_;
  Reporter reporter_;
};

}  // namespace autoware_self_test_infrastructure
