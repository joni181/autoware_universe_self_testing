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

#include "autoware_self_test_infrastructure/self_test_registry.hpp"
#include "autoware_self_test_infrastructure/self_test_service.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <string>

namespace autoware_self_test_infrastructure
{

class SelfTestInfrastructure final
{
public:
  SelfTestInfrastructure();
  explicit SelfTestInfrastructure(const std::shared_ptr<ISelfTestRegistry> & registry);

  std::shared_ptr<ISelfTestRegistry> get_registry() const;

  void initialize_test_api(
    rclcpp::Node & node, const std::string & service_name = "/self_test/run");

private:
  std::shared_ptr<ISelfTestRegistry> registry_;
  std::unique_ptr<SelfTestService> service_;
};

}  // namespace autoware_self_test_infrastructure

