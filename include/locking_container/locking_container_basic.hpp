#pragma once

#include <stdexcept>

#include "locking_container/detail/macros.hpp"
#include "locking_container/detail/mutex_type.hpp"

namespace burda
{
namespace stl
{
/**
 * Class that extends STL container in order to provide thread-safety using read-write locking mechanism
 * @tparam T STL container (e.g. std::vector, std::map, ...)
 */
template <typename T>
class locking_container_basic : public T
{
public:
    using T::T;

    locking_container_basic() = default;

    ~locking_container_basic() = default;

    locking_container_basic(const locking_container_basic &) = delete;

    locking_container_basic & operator=(const locking_container_basic &) = delete;

    locking_container_basic(locking_container_basic &&) = delete;

    locking_container_basic & operator=(locking_container_basic &&) = delete;

    template <typename U>
    void read_lock(U && callback) const
    {
        BURDA_LC_LOCK_SHARED

        callback();
    }

    template <typename U>
    void write_lock(U && callback) const
    {
        BURDA_LC_LOCK_EXCLUSIVE

        callback();
    }

protected:
    mutable detail::mutex_type read_write_mutex;
};
}
}
