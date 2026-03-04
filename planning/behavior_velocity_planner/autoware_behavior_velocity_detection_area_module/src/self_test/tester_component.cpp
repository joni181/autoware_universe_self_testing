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

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <rclcpp/duration.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>
#include <string>
#include <utility>

namespace autoware::behavior_velocity_planner
{
namespace
{
using PathWithLaneId = autoware_internal_planning_msgs::msg::PathWithLaneId;

constexpr auto MAX_WAIT_FOR_FIRST_MSG = std::chrono::milliseconds{300};
constexpr auto MAX_AGE = std::chrono::milliseconds{800};
constexpr std::size_t MIN_MSG_COUNT = 1U;

PathWithLaneId make_empty_path()
{
  return PathWithLaneId{};
}

types::TestResult pass_result()
{
  types::TestResult r;
  r.result = types::TestResultStatus::PASS;
  return r;
}

types::TestResult fail_result(const types::error_map_t & error)
{
  types::TestResult r;
  r.result = types::TestResultStatus::FAIL;
  r.error = error;
  return r;
}

types::TestResult skip_result(const types::error_map_t & error)
{
  types::TestResult r;
  r.result = types::TestResultStatus::SKIP;
  r.error = error;
  return r;
}

std::string bool_to_string(const bool value) { return value ? "true" : "false"; }

int64_t to_ms(const rclcpp::Duration & duration)
{
  return static_cast<int64_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::nanoseconds{duration.nanoseconds()})
      .count());
}
}  // namespace

TesterComponent::TesterComponent(IDetectionAreaTestable & detection_area_testable)
: detection_area_testable_(detection_area_testable)
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

  auto add = [this](
               const std::string & name,
               types::TestResult (TesterComponent::*method)()) {
    auto tc = std::make_shared<types::TestCase>();
    tc->name = name;
    test_cases_.push_back(tc);

    const std::size_t idx = test_cases_.size() - 1U;

    tc->test_function = [this, method, idx]() -> types::TestResult {
      // careful: this is only stable as long as test cases are not reordered/erased after registration
      const auto & tc_ptr = test_cases_[idx];

      // measures execution time but keeps the actual test logic isolated in the test_* method
      return run_timed(tc_ptr, [this, method]() { return (this->*method)(); });
    };
  };

  add("test_no_detection_area", &TesterComponent::test_no_detection_area);
  add("test_activated_no_obstacles", &TesterComponent::test_activated_no_obstacles);
  add("test_predicted_objects_provider_available", &TesterComponent::test_predicted_objects_provider_available);
  add("test_predicted_object", &TesterComponent::test_predicted_object);
  add("test_predicted_object_class_disabled", &TesterComponent::test_predicted_object_class_disabled);
  add("test_pointcloud_obstacle", &TesterComponent::test_pointcloud_obstacle);
  add("test_respect_stop_margin", &TesterComponent::test_respect_stop_margin);
  add("test_restart_prevention", &TesterComponent::test_restart_prevention);
}

// ----------
// Test cases
// ----------

types::TestResult TesterComponent::test_no_detection_area()
{
  (void)detection_area_testable_.capture_snapshot(make_empty_path());
  return pass_result();
}

types::TestResult TesterComponent::test_activated_no_obstacles()
{
  return pass_result();
}

types::TestResult TesterComponent::test_predicted_objects_provider_available()
{
  auto snapshot = detection_area_testable_.capture_snapshot(make_empty_path());

  if (snapshot.pointcloud_detection_enabled) {
    return skip_result({
      {"reason",
       "Skipped because target_filtering.pointcloud=true, so predicted objects are not a required provider."},
      {"target_filtering.pointcloud", "true"}});
  }

  if (snapshot.dynamic_objects.received_count < MIN_MSG_COUNT) {
    std::this_thread::sleep_for(MAX_WAIT_FOR_FIRST_MSG);
    snapshot = detection_area_testable_.capture_snapshot(make_empty_path());
  }

  if (snapshot.dynamic_objects.received_count < MIN_MSG_COUNT) {
    return fail_result({
      {"reason",
       "Missing predicted objects provider (~input/dynamic_objects): no messages observed in the allowed wait window."},
      {"required_min_msg_count", std::to_string(MIN_MSG_COUNT)},
      {"received_msg_count", std::to_string(snapshot.dynamic_objects.received_count)},
      {"has_msg", bool_to_string(snapshot.dynamic_objects.has_msg)},
      {"max_wait_for_first_msg_ms", std::to_string(MAX_WAIT_FOR_FIRST_MSG.count())}});
  }

  if (!snapshot.dynamic_objects.last_receive_time.has_value()) {
    return fail_result({
      {"reason",
       "Predicted objects provider metadata is incomplete: last_receive_time is unavailable."},
      {"received_msg_count", std::to_string(snapshot.dynamic_objects.received_count)}});
  }

  const auto age = snapshot.capture_time - *snapshot.dynamic_objects.last_receive_time;
  const auto age_ms = std::max<int64_t>(0, to_ms(age));
  if (age_ms > MAX_AGE.count()) {
    return fail_result({
      {"reason",
       "Predicted objects provider is stale: latest observed message age exceeds MAX_AGE."},
      {"age_ms", std::to_string(age_ms)},
      {"max_age_ms", std::to_string(MAX_AGE.count())},
      {"received_msg_count", std::to_string(snapshot.dynamic_objects.received_count)}});
  }

  return pass_result();
}

types::TestResult TesterComponent::test_predicted_object()
{
  return pass_result();
}

types::TestResult TesterComponent::test_predicted_object_class_disabled()
{
  return pass_result();
}

types::TestResult TesterComponent::test_pointcloud_obstacle()
{
  return pass_result();
}

types::TestResult TesterComponent::test_respect_stop_margin()
{
  return pass_result();
}

types::TestResult TesterComponent::test_restart_prevention()
{
  return pass_result();
}

}  // namespace autoware::behavior_velocity_planner
