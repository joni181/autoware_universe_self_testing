#pragma once

#include "autoware_self_test_infrastructure/self_test_controller.hpp"
#include "autoware_self_test_infrastructure/self_test_registry.hpp"

#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>

#include <memory>
#include <string>
#include <functional>

namespace autoware_self_test_infrastructure
{

class SelfTestService final
{
public:
  explicit SelfTestService(rclcpp::Node & node)
  : controller_(SelfTestRegistry::instance())
  {
    using std::placeholders::_1;
    using std::placeholders::_2;

    srv_ = node.create_service<std_srvs::srv::Trigger>(
      "/self_test/run",
      std::bind(&SelfTestService::on_trigger, this, _1, _2));

    RCLCPP_INFO(node.get_logger(), "Self-test service ready: /self_test/run");
  }

private:
  void on_trigger(
    const std::shared_ptr<std_srvs::srv::Trigger::Request> /*req*/,
    std::shared_ptr<std_srvs::srv::Trigger::Response> resp)
  {
    const std::string report_json = controller_.run();
    resp->success = !report_json.empty();  // client reads validation_decision from JSON
    resp->message = report_json;
  }

  SelfTestController controller_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_;
};

}  // namespace autoware_self_test_infrastructure
