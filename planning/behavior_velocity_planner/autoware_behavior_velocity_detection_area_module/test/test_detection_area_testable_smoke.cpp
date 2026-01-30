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

#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

#include "scene.hpp"
#include "self_test/i_detection_area_testable.hpp"

namespace autoware::behavior_velocity_planner
{

// Compile-time checks
static_assert(
  std::is_same_v<decltype(std::declval<DetectionAreaModule &>().testable()), IDetectionAreaTestable &>,
  "DetectionAreaModule::testable() must return IDetectionAreaTestable&");

static_assert(
  std::is_same_v<decltype(std::declval<const DetectionAreaModule &>().testable()), const IDetectionAreaTestable &>,
  "DetectionAreaModule::testable() const must return const IDetectionAreaTestable&");

TEST(DetectionAreaTestableSmoke, BuildsAndLinks)
{
  // No runtime assertions needed; compilation + linking is the purpose
  SUCCEED();
}

}  // namespace autoware::behavior_velocity_planner
