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

#include "autoware_self_test_infrastructure/self_test_service.hpp"

#include <stdexcept>

namespace autoware_self_test_infrastructure
{
namespace
{
ISelfTestRegistry & ensure_registry(const std::shared_ptr<ISelfTestRegistry> & registry)
{
  if (!registry) {
    throw std::invalid_argument("SelfTestService requires a non-null registry.");
  }
  return *registry;
}
}  // namespace

SelfTestService::SelfTestService(
  rclcpp::Node & node, const std::shared_ptr<ISelfTestRegistry> & registry,
  const std::string & service_name)
: registry_(registry), controller_(ensure_registry(registry))
{
  using std::placeholders::_1;
  using std::placeholders::_2;

  srv_ = node.create_service<std_srvs::srv::Trigger>(
    service_name, std::bind(&SelfTestService::on_trigger, this, _1, _2));

  RCLCPP_INFO(node.get_logger(), "Self-test service ready: %s", service_name.c_str());
}

}  // namespace autoware_self_test_infrastructure
