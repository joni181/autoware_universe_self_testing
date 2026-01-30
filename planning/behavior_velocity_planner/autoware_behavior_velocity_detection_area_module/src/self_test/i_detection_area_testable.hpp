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

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>

#include <cstddef>
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
};
}  // namespace autoware::behavior_velocity_planner

#endif  // I_DETECTION_AREA_TESTABLE_HPP_