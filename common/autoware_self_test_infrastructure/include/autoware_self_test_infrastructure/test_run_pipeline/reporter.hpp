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

#include <autoware_self_test_types/types.hpp>

#include<optional>
#include<string>
#include<unordered_map>

namespace autoware_self_test_infrastructure
{
namespace types = autoware_self_test_types;

class IReporter
{
public:
  virtual ~IReporter() = default;

  virtual std::string build(
    const types::EvaluationResult & evaluation_result,
    const types::TestRunResult & test_run_result) const = 0;
};

class Reporter final : public IReporter
{
public:
  std::string build(
    const types::EvaluationResult & evaluation_result,
    const types::TestRunResult & test_run_result) const override;
};

}  // namespace autoware_self_test_infrastructure