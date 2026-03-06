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
  explicit SelfTestService(
    rclcpp::Node & node, const std::shared_ptr<ISelfTestRegistry> & registry,
    const std::string & service_name = "/self_test/run");

private:
  void on_trigger(
    const std::shared_ptr<std_srvs::srv::Trigger::Request> /*req*/,
    std::shared_ptr<std_srvs::srv::Trigger::Response> resp)
  {
    const std::string report_json = controller_.run();
    resp->success = !report_json.empty();  // client reads validation_decision from JSON
    resp->message = report_json;
  }

  std::shared_ptr<ISelfTestRegistry> registry_;
  SelfTestController controller_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr srv_;
};

}  // namespace autoware_self_test_infrastructure
