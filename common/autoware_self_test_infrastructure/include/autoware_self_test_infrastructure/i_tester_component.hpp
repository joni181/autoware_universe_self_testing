#pragma once

#include <autoware_self_test_types/types.hpp>

#include <memory>
#include <vector>

namespace autoware_self_test_infrastructure
{
namespace types = autoware_self_test_types;

class ITesterComponent
{
public:
    virtual ~ITesterComponent() = default;

    // Non-owning access to registered test cases
    // Contract: caller must not retain references beyond the lifetime of this component.
    virtual const std::vector<std::shared_ptr<const types::TestCase>> & get_test_cases() const = 0;
};

} // namespace autoware_self_test_infrastructure
