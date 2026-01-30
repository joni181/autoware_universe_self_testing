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

#include "tester_component.hpp"

#include <memory>
#include <string>
#include <utility>

namespace autoware::behavior_velocity_planner
{

TesterComponent::TesterComponent(IDetectionAreaTestable & detection_area_testable) : detection_area_testable_(detection_area_testable)
{
    register_test_cases();
}

const std::vector<std::shared_ptr<const types::TestCase>> & TesterComponent::get_test_cases() const
{
    return test_cases_;
}

void TesterComponent::register_test_cases()
{
    test_cases_.clear();

    auto add = [this](types::TestResult (TesterComponent::*method)()) {
        auto tc = std::make_shared<types::TestCase>();
        test_cases_.push_back(tc);

        const std::size_t idx = test_cases_.size() - 1U;

        /*
        Bind an invokable into the TestCase. We capture:
        - mmethod: which TersterComponent::test_*
        - idx: to retrieve the corresponding TestCase to link the result / get the timing
        - this: to access member functions
        */
        tc->test_function = [this, method, idx]() -> types::TestResult {
            // careful: this is only stable as long as test cases are not reordered/erased after registration
            const auto & tc_ptr = test_cases_[idx];

            // measures execution time but keeps the actual test logic isolated in the test_* method
            return run_timed(tc_ptr, [this, method]() {
                return (this->*method)();
            });
        };
    };

    add(&TesterComponent::test_no_detection_area);
    add(&TesterComponent::test_activated_no_obstacles);
    add(&TesterComponent::test_predicted_object);
    add(&TesterComponent::test_predicted_object_class_disabled);
    add(&TesterComponent::test_pointcloud_obstacle);
    add(&TesterComponent::test_respect_stop_margin);
    add(&TesterComponent::test_restart_prevention);
}

// ----------
// Test cases
// ----------

types::TestResult TesterComponent::test_no_detection_area()
{
    types::TestResult r;
    r.passed = true;
    r.coverage = 0.0;
    return r;
}

types::TestResult TesterComponent::test_activated_no_obstacles()
{
  types::TestResult r;
  r.passed = true;
  r.coverage = 0.0;
  return r;
}

types::TestResult TesterComponent::test_predicted_object()
{
  types::TestResult r;
  r.passed = true;
  r.coverage = 0.0;
  return r;
}

types::TestResult TesterComponent::test_predicted_object_class_disabled()
{
  types::TestResult r;
  r.passed = true;
  r.coverage = 0.0;
  return r;
}

types::TestResult TesterComponent::test_pointcloud_obstacle()
{
  types::TestResult r;
  r.passed = true;
  r.coverage = 0.0;
  return r;
}

types::TestResult TesterComponent::test_respect_stop_margin()
{
  types::TestResult r;
  r.passed = true;
  r.coverage = 0.0;
  return r;
}

types::TestResult TesterComponent::test_restart_prevention()
{
  types::TestResult r;
  r.passed = true;
  r.coverage = 0.0;
  return r;
}

}  // namespace autoware::behavior_velocity_planner