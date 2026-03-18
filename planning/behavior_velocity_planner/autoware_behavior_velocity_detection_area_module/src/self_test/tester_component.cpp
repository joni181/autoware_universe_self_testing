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
#include <autoware_perception_msgs/msg/object_classification.hpp>
#include <autoware_perception_msgs/msg/predicted_object.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/shape.hpp>
#include <rclcpp/duration.hpp>

#include <lanelet2_core/geometry/Polygon.h>

#include <algorithm>
#include <chrono>
#include <cmath>
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

  add("test_predicted_objects_provider_available", &TesterComponent::test_predicted_objects_provider_available);
  add("test_detection_area_geometric_misfit", &TesterComponent::test_detection_area_geometric_misfit);
}

// ----------
// Test cases
// ----------

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

types::TestResult TesterComponent::test_detection_area_geometric_misfit()
{
  // 1. Get detection area polygons from the lanelet map
  const auto detection_areas = detection_area_testable_.get_detection_areas();
  if (detection_areas.empty()) {
    return skip_result({
      {"reason", "No detection area polygons available from the regulatory element."}});
  }

  // 2. Compute centroid of the first detection area polygon
  const auto & poly = detection_areas.front();
  if (poly.size() < 3) {
    return skip_result({
      {"reason", "Detection area polygon has fewer than 3 vertices."},
      {"vertex_count", std::to_string(poly.size())}});
  }

  double cx = 0.0, cy = 0.0, cz = 0.0;
  for (const auto & pt : poly) {
    cx += pt.x();
    cy += pt.y();
    cz += pt.z();
  }
  const double n = static_cast<double>(poly.size());
  cx /= n;
  cy /= n;
  cz /= n;

  // 3. Determine which object class to use for the synthetic object
  const auto enabled_label = detection_area_testable_.get_first_enabled_object_label();
  if (!enabled_label.has_value()) {
    return skip_result({
      {"reason",
       "No predicted-object class is enabled in target_filtering. "
       "The module is configured for pointcloud-only detection; "
       "geometric misfit test for predicted objects does not apply."}});
  }

  // 4. Read perception offset parameter (for fault injection demo)
  const double offset = detection_area_testable_.get_self_test_perception_offset();

  // Apply offset to the x-coordinate of the centroid
  const double injected_x = cx + offset;
  const double injected_y = cy;

  // 5. Create a synthetic PredictedObject at the (possibly offset) centroid
  autoware_perception_msgs::msg::PredictedObjects synthetic_objects;

  autoware_perception_msgs::msg::PredictedObject obj;
  obj.kinematics.initial_pose_with_covariance.pose.position.x = injected_x;
  obj.kinematics.initial_pose_with_covariance.pose.position.y = injected_y;
  obj.kinematics.initial_pose_with_covariance.pose.position.z = cz;
  obj.kinematics.initial_pose_with_covariance.pose.orientation.w = 1.0;

  // Give it a small box shape (1m x 1m x 1m) so the polygon intersection works
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 1.0;
  obj.shape.dimensions.y = 1.0;
  obj.shape.dimensions.z = 1.0;

  // Classify with the first enabled object class
  autoware_perception_msgs::msg::ObjectClassification cls;
  cls.label = enabled_label.value();
  cls.probability = 1.0;
  obj.classification.push_back(cls);

  synthetic_objects.objects.push_back(obj);

  // 6. Run the module's obstacle detection on the synthetic object
  const auto detected = detection_area_testable_.run_obstacle_detection(synthetic_objects);

  // 7. Evaluate
  if (detected.has_value()) {
    return pass_result();
  }

  return fail_result({
    {"reason",
     "Synthetic object placed at detection area centroid was NOT detected by the module's "
     "obstacle detection logic. This indicates a geometric misfit between perception output "
     "and the map's detection area polygon."},
    {"centroid_x", std::to_string(cx)},
    {"centroid_y", std::to_string(cy)},
    {"injected_x", std::to_string(injected_x)},
    {"injected_y", std::to_string(injected_y)},
    {"perception_offset_m", std::to_string(offset)}});
}

}  // namespace autoware::behavior_velocity_planner
