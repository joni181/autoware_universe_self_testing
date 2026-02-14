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

#include "autoware_self_test_infrastructure/test_run_pipeline/reporter.hpp"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <optional>
#include <ctime>

namespace autoware_self_test_infrastructure
{
namespace types = autoware_self_test_types;

namespace
{
// ------------------------ JSON helpers ------------------------

std::string json_escape(const std::string & s)
{
  std::ostringstream oss;
  for (const char c : s) {
    switch (c) {
      case '\\': oss << "\\\\"; break;
      case '"': oss << "\\\""; break;
      case '\b': oss << "\\b"; break;
      case '\f': oss << "\\f"; break;
      case '\n': oss << "\\n"; break;
      case '\r': oss << "\\r"; break;
      case '\t': oss << "\\t"; break;
      default:
        // Control chars must be escaped.
        if (static_cast<unsigned char>(c) < 0x20U) {
          oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
              << static_cast<int>(static_cast<unsigned char>(c))
              << std::dec << std::setw(0);
        } else {
          oss << c;
        }
    }
  }
  return oss.str();
}

std::string iso8601_utc(std::chrono::system_clock::time_point tp)
{
  using namespace std::chrono;

  const auto t = system_clock::to_time_t(tp);
  const auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;

  std::tm tm{};
#if defined(_WIN32)
  gmtime_s(&tm, &t);
#else
  gmtime_r(&t, &tm);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
      << '.' << std::setw(3) << std::setfill('0') << ms.count()
      << 'Z';
  return oss.str();
}

std::string bool_json(bool v) { return v ? "true" : "false"; }

std::string get_test_case_name(const std::shared_ptr<const types::TestCase> & tc, std::size_t idx)
{
  if (!tc) {
    return "test_" + std::to_string(idx);
  }

  if (!tc->name.empty()) {
    return tc->name;
  }

  return "missing_name_" + std::to_string(idx);
}

std::size_t count_failed(const std::vector<types::TestResult> & results)
{
  std::size_t failed = 0;
  for (const auto & tr : results) {
    if (!tr.passed) {
      ++failed;
    }
  }
  return failed;
}

std::size_t count_passed(const std::vector<types::TestResult> & results)
{
  std::size_t passed = 0;
  for (const auto & tr : results) {
    if (tr.passed) {
      ++passed;
    }
  }
  return passed;
}

}  // namespace

std::string Reporter::build(
  const types::EvaluationResult & evaluation_result,
  const types::TestRunResult & test_run_result) const
{
  using namespace std::chrono;

  const double run_elapsed_sec = std::max(0.0, test_run_result.elapsed_time_sec);

  const std::size_t total = test_run_result.test_results.size();
  const std::size_t failed = count_failed(test_run_result.test_results);
  const std::size_t passed = count_passed(test_run_result.test_results);

  // Report id uses a timestamp to avoid collisions and to be human-friendly.
  const std::string report_id = std::string("self_test_report_") + iso8601_utc(system_clock::now());

  std::ostringstream out;
  out.setf(std::ios::fixed);
  out << std::setprecision(6);

  out << "{";
  out << "\"report_id\":\"" << json_escape(report_id) << "\",";
  out << "\"schema_version\":\"1.0\",";
  out << "\"elapsed_time_sec\":" << run_elapsed_sec << ",";

  out << "\"total_count\":" << total << ",";
  out << "\"passed_count\":" << passed << ",";
  out << "\"failed_count\":" << failed << ",";

  out << "\"validation_decision\":" << bool_json(evaluation_result.validation_decision) << ",";
  out << "\"validation_reason\":\"" << json_escape(evaluation_result.validation_reason) << "\",";

  // Detailed per-test results as an array.
  out << "\"test_results\":[";

  for (std::size_t i = 0; i < test_run_result.test_results.size(); ++i) {
    const auto & tr = test_run_result.test_results[i];

    const double test_elapsed = std::max(0.0, tr.elapsed_time_sec);

    if (i > 0) out << ",";

    out << "{";
    out << "\"index\":" << i << ",";
    out << "\"name\":\"" << json_escape(get_test_case_name(tr.test_case, i)) << "\",";
    out << "\"passed\":" << bool_json(tr.passed) << ",";
    out << "\"elapsed_time_sec\":" << test_elapsed;

    // Optional error map if present
    if (tr.error.has_value()) {
      out << ",\"error\":{";
      std::vector<std::pair<std::string, std::string>> err_kv;
      err_kv.reserve(tr.error->size());
      for (const auto & it : *tr.error) {
        err_kv.emplace_back(it.first, it.second);
      }
      std::sort(err_kv.begin(), err_kv.end(), [](const auto & a, const auto & b) { return a.first < b.first; });

      for (std::size_t e = 0; e < err_kv.size(); ++e) {
        if (e > 0) out << ",";
        out << "\"" << json_escape(err_kv[e].first) << "\":\"" << json_escape(err_kv[e].second) << "\"";
      }
      out << "}";
    }

    out << "}";
  }

  out << "]";
  out << "}";

  return out.str();
}

}  // namespace autoware_self_test_infrastructure
