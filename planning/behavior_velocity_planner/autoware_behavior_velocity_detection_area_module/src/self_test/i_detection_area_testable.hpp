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

#ifndef I_DETECTION_AREA_TESTABLE_HPP_
#define I_DETECTION_AREA_TESTABLE_HPP_

#include "detection_area_snapshot.hpp"

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <autoware_perception_msgs/msg/predicted_object.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>

#include <lanelet2_core/primitives/Polygon.h>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace autoware::behavior_velocity_planner
{
class IDetectionAreaTestable
{
public:
  // Mirror of the module's logical state (kept here to avoid depending on scene.hpp)
  enum class State { GO, STOP };

  virtual ~IDetectionAreaTestable() = default;

  // snapshot for isolation
  virtual self_test::DetectionAreaSnapshot capture_snapshot(
    const autoware_internal_planning_msgs::msg::PathWithLaneId & current_path) const = 0;

  virtual void print_detected_obstacle(
    const std::vector<geometry_msgs::msg::Point> & obstacle_points,
    const geometry_msgs::msg::Pose & self_pose) const = 0;

  virtual void finalizeStopPoint(
    autoware_internal_planning_msgs::msg::PathWithLaneId * path,
    const geometry_msgs::msg::Pose & stop_pose,
    const geometry_msgs::msg::Pose & modified_stop_pose,
    size_t modified_stop_line_seg_idx,
    const geometry_msgs::msg::Pose & self_pose,
    const std::string & detection_source,
    const std::string & policy_name,
    State prev_state) = 0;

  virtual bool handleUnstoppableGoPolicy() = 0;

  virtual bool handleUnstoppableForceStopPolicy(
    autoware_internal_planning_msgs::msg::PathWithLaneId * path,
    const geometry_msgs::msg::Pose & stop_pose,
    const geometry_msgs::msg::Pose & modified_stop_pose,
    size_t modified_stop_line_seg_idx,
    const geometry_msgs::msg::Pose & self_pose,
    const std::string & detection_source) = 0;

  virtual bool handleUnstoppableStopAfterLinePolicy(
    autoware_internal_planning_msgs::msg::PathWithLaneId * path,
    const autoware_internal_planning_msgs::msg::PathWithLaneId & original_path,
    const geometry_msgs::msg::Pose & stop_pose,
    geometry_msgs::msg::Pose & modified_stop_pose,
    size_t & modified_stop_line_seg_idx,
    const geometry_msgs::msg::Pose & self_pose,
    double current_velocity,
    double stop_dist,
    const std::string & detection_source) = 0;

  /// @brief Get the detection area polygons from the lanelet regulatory element
  virtual lanelet::ConstPolygons3d get_detection_areas() const = 0;

  /// @brief Run the module's obstacle detection logic on the given predicted objects
  virtual std::optional<autoware_perception_msgs::msg::PredictedObject> run_obstacle_detection(
    const autoware_perception_msgs::msg::PredictedObjects & objects) const = 0;

  /// @brief Read the self-test perception offset parameter (dynamic, for fault injection demo)
  virtual double get_self_test_perception_offset() const = 0;

  /// @brief Return the label of the first enabled predicted-object class, or nullopt if none
  virtual std::optional<uint8_t> get_first_enabled_object_label() const = 0;
};
}  // namespace autoware::behavior_velocity_planner

#endif  // I_DETECTION_AREA_TESTABLE_HPP_
