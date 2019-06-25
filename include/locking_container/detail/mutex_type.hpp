#pragma once

#include <mutex>
#include <shared_mutex>

namespace burda
{
namespace stl
{
namespace detail
{
// C++17 and higher
#if __cplusplus > 201402L
    using mutex_type = std::shared_mutex;
#else
// C++14
    using mutex_type = std::shared_timed_mutex;
#endif
}
}
}
