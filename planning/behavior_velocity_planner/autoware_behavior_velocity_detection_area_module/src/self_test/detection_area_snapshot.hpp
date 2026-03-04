#pragma once

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <geometry_msgs/msg/pose.hpp>

#include <memory>
#include <optional>

#include <sensor_msgs/msg/point_cloud2.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>

namespace autoware::behavior_velocity_planner::self_test
{
struct DetectionAreaSnapshot
{
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  geometry_msgs::msg::Pose self_pose{};
  double current_velocity_mps{0.0};

  // Keep shared_ptr to message snapshots (immutable)
  // We may need deep copies later -> copy the message objects
  std::shared_ptr<const sensor_msgs::msg::PointCloud2> no_ground_pointcloud;
  std::shared_ptr<const autoware_perception_msgs::msg::PredictedObjects> predicted_objects;
}
}  // namespace autoware::behavior_velocity_planner::self_test
