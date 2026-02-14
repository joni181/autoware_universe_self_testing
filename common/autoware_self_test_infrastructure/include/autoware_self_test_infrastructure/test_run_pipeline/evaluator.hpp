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

#include <string>
#include <utility>

namespace autoware_self_test_infrastructure
{
namespace types = autoware_self_test_types;

struct ValidationPolicy
{
  // TODO(jlippss): here policy elements can be added
};

class IEvaluation
{
public:
  virtual ~IEvaluation() = default;

  virtual types::EvaluationResult evaluate(const types::TestRunResult & test_run_result) const = 0;
};

class Evaluator final : public IEvaluation
{
public:
  explicit Evaluator(const ValidationPolicy & policy = ValidationPolicy{});

  types::EvaluationResult evaluate(const types::TestRunResult & test_run_result) const override;

  void set_validation_policy(const ValidationPolicy & policy);
  const ValidationPolicy & get_validation_policy() const;

private:
  ValidationPolicy policy_;

  std::pair<bool, std::string> validate_(const types::TestRunResult & test_run_result) const;
};

}  // namespace autoware_self_test_infrastructure