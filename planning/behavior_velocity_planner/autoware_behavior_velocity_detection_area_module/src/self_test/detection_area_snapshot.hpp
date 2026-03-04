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

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <rclcpp/time.hpp>

#include <cstddef>
#include <memory>
#include <optional>

#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace autoware::behavior_velocity_planner::self_test
{
struct DetectionAreaSnapshot
{
  struct DynamicObjectsState
  {
    bool has_msg{false};
    std::optional<rclcpp::Time> last_receive_time{};
    std::size_t received_count{0};
  };

  rclcpp::Time capture_time{};

  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  geometry_msgs::msg::Pose self_pose{};
  double current_velocity_mps{0.0};
  bool pointcloud_detection_enabled{true};
  DynamicObjectsState dynamic_objects{};

  // Keep shared_ptr to message snapshots (immutable)
  // We may need deep copies later -> copy the message objects
  pcl::PointCloud<pcl::PointXYZ>::ConstPtr no_ground_pointcloud;
  std::shared_ptr<const autoware_perception_msgs::msg::PredictedObjects> predicted_objects;
};
}  // namespace autoware::behavior_velocity_planner::self_test
