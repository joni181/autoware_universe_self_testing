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

#include "autoware_self_test_infrastructure/self_test_infrastructure.hpp"

#include <stdexcept>

namespace autoware_self_test_infrastructure
{
namespace
{
std::shared_ptr<ISelfTestRegistry> ensure_registry(
  const std::shared_ptr<ISelfTestRegistry> & registry)
{
  if (!registry) {
    throw std::invalid_argument("SelfTestInfrastructure requires a non-null registry.");
  }
  return registry;
}
}  // namespace

SelfTestInfrastructure::SelfTestInfrastructure()
: registry_(std::make_shared<SelfTestRegistry>())
{
}

SelfTestInfrastructure::SelfTestInfrastructure(const std::shared_ptr<ISelfTestRegistry> & registry)
: registry_(ensure_registry(registry))
{
}

std::shared_ptr<ISelfTestRegistry> SelfTestInfrastructure::get_registry() const
{
  return registry_;
}

void SelfTestInfrastructure::initialize_test_api(
  rclcpp::Node & node, const std::string & service_name)
{
  if (service_) {
    return;
  }

  service_ = std::make_unique<SelfTestService>(node, registry_, service_name);
}

}  // namespace autoware_self_test_infrastructure

