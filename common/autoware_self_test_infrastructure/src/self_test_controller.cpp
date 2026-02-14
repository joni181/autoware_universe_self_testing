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

#include "autoware_self_test_infrastructure/self_test_controller.hpp"

#include "autoware_self_test_infrastructure/test_run_pipeline/evaluator.hpp"
#include "autoware_self_test_infrastructure/test_run_pipeline/reporter.hpp"
#include "autoware_self_test_infrastructure/test_run_pipeline/test_executor.hpp"

#include <string>

namespace autoware_self_test_infrastructure
{

SelfTestController::SelfTestController(SelfTestRegistry & registry)
: executor_(registry), evaluator_(ValidationPolicy{}), reporter_()
{
}

std::string SelfTestController::run()
{
  const auto test_run_result = executor_.execute();
  const auto evaluation_result = evaluator_.evaluate(test_run_result);
  return reporter_.build(evaluation_result, test_run_result);
}

}  // namespace autoware_self_test_infrastructure