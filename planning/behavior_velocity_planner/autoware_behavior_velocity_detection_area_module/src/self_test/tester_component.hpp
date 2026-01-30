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

#include "i_detection_area_testable.hpp"

#include <autoware_self_test_types/types.hpp>
#include <autoware_self_test_infrastructure/i_tester_component.hpp>

#include <chrono>
#include <memory>
#include <utility>
#include <vector>

namespace autoware::behavior_velocity_planner
{
namespace types = autoware_self_test_types;
namespace infra = autoware_self_test_infrastructure;

class TesterComponent final : public infra::ITesterComponent
{
public:
    explicit TesterComponent(IDetectionAreaTestable & detection_area_testable);

    const std::vector<std::shared_ptr<const types::TestCase>> & get_test_cases() const override;

    // Concrete test case operations
    types::TestResult test_no_detection_area();
    types::TestResult test_activated_no_obstacles();
    types::TestResult test_predicted_object();
    types::TestResult test_predicted_object_class_disabled();
    types::TestResult test_pointcloud_obstacle();
    types::TestResult test_respect_stop_margin();
    types::TestResult test_restart_prevention();
    // ...

private:
    // non-owning reference to the testable component
    IDetectionAreaTestable & detection_area_testable_;

    // registry of test cases (owned by this tester component)
    // stored as shared_ptr so TestResult can reference them without copying
    std::vector<std::shared_ptr<const types::TestCase>> test_cases_;

    // register all test cases exactly once (constructor calls this)
    void register_test_cases();

    // measure execution time of a test function
    template <typename Fn>
    types::TestResult run_timed(const std::shared_ptr<const types::TestCase> & tc, Fn && fn)
    {
        const auto start = std::chrono::steady_clock::now();
        types::TestResult result = std::forward<Fn>(fn)();
        const auto end = std::chrono::steady_clock::now();

        // TestResult stores shared pointer
        result.test_case = tc;
        result.elapsed_time_sec = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

        return result;
    }

    // ==== helpers ====
    // e.g. mock_path_with_lane_id(...)
};
}  // namespace autoware::behavior_velocity_planner
