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

#include "self_test/i_detection_area_testable.hpp"
#include "self_test/tester_component.hpp"

namespace bvp = autoware::behavior_velocity_planner;
namespace msgs = autoware_internal_planning_msgs::msg;

// Minimal stub: the current dummy test_* methods in TesterComponent should not call into the
// module adapter yet. If any of these get called unexpectedly, fail hard.
class DummyDetectionAreaTestable final : public bvp::IDetectionAreaTestable
{
public:
  void print_detected_obstacle(
    const std::vector<geometry_msgs::msg::Point> & /*obstacle_points*/,
    const geometry_msgs::msg::Pose & /*self_pose*/) const override
  {
    FAIL() << "print_detected_obstacle() should not be called by dummy TesterComponent tests.";
  }

  void finalizeStopPoint(
    msgs::PathWithLaneId * /*path*/,
    const geometry_msgs::msg::Pose & /*stop_pose*/,
    const geometry_msgs::msg::Pose & /*modified_stop_pose*/,
    size_t /*modified_stop_line_seg_idx*/,
    const geometry_msgs::msg::Pose & /*self_pose*/,
    const std::string & /*detection_source*/,
    const std::string & /*policy_name*/,
    State /*prev_state*/) override
  {
    FAIL() << "finalizeStopPoint() should not be called by dummy TesterComponent tests.";
  }

  bool handleUnstoppableGoPolicy() override
  {
    ADD_FAILURE() << "handleUnstoppableGoPolicy() should not be called by dummy TesterComponent tests.";
    return false;
  }

  bool handleUnstoppableForceStopPolicy(
    msgs::PathWithLaneId * /*path*/,
    const geometry_msgs::msg::Pose & /*stop_pose*/,
    const geometry_msgs::msg::Pose & /*modified_stop_pose*/,
    size_t /*modified_stop_line_seg_idx*/,
    const geometry_msgs::msg::Pose & /*self_pose*/,
    const std::string & /*detection_source*/) override
  {
    ADD_FAILURE() << "handleUnstoppableForceStopPolicy() should not be called by dummy TesterComponent tests.";
    return false;
  }

  bool handleUnstoppableStopAfterLinePolicy(
    msgs::PathWithLaneId * /*path*/,
    const msgs::PathWithLaneId & /*original_path*/,
    const geometry_msgs::msg::Pose & /*stop_pose*/,
    geometry_msgs::msg::Pose & /*modified_stop_pose*/,
    size_t & /*modified_stop_line_seg_idx*/,
    const geometry_msgs::msg::Pose & /*self_pose*/,
    double /*current_velocity*/,
    double /*stop_dist*/,
    const std::string & /*detection_source*/) override
  {
    ADD_FAILURE() << "handleUnstoppableStopAfterLinePolicy() should not be called by dummy TesterComponent tests.";
    return false;
  }
};

TEST(detection_area_tester_component, registers_cases_and_invokes)
{
  DummyDetectionAreaTestable stub;
  bvp::TesterComponent tester(stub);

  const auto & cases = tester.get_test_cases();
  ASSERT_FALSE(cases.empty());

  for (const auto & tc_ptr : cases) {
    ASSERT_TRUE(tc_ptr);
    ASSERT_TRUE(static_cast<bool>(tc_ptr->test_function));

    const auto result = tc_ptr->test_function();

    // run_timed() should attach metadata to the result
    ASSERT_TRUE(result.test_case);
    EXPECT_EQ(result.test_case, tc_ptr);

    // elapsed time should be set (>= 0)
    EXPECT_GE(result.elapsed_time_sec, 0.0);

    // current dummy tests return "passed"; keep this as a sanity check
    EXPECT_TRUE(result.passed);
  }
}
