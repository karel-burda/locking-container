#pragma once

#include "locking_container/locking_container_basic.hpp"

namespace burda
{
namespace stl
{
/**
 * Implements thread-safe variants (as new methods) of all containers' methods,
 * list available at: https://en.cppreference.com/w/cpp/container
 * @see "locking_container_basic"
 */
template <typename T>
class locking_container : public locking_container_basic<T>
{
public:
    using locking_container_basic<T>::locking_container_basic;

    /// other
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(assign)

    /// iterators
    BURDA_LC_IMPLEMENT_ITERATOR(begin)
    BURDA_LC_IMPLEMENT_CITERATOR(cbegin)
    BURDA_LC_IMPLEMENT_ITERATOR(end)
    BURDA_LC_IMPLEMENT_CITERATOR(cend)
    BURDA_LC_IMPLEMENT_ITERATOR(rbegin)
    BURDA_LC_IMPLEMENT_CITERATOR(crbegin)
    BURDA_LC_IMPLEMENT_ITERATOR(rend)
    BURDA_LC_IMPLEMENT_CITERATOR(crend)

    /// element access
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(at)

    // TODO: use size_type
    auto & operator[](const size_t pos)
    {
        BURDA_LC_LOCK_EXCLUSIVE

        return T::operator[](pos);
    }

    constexpr const auto & operator[](const size_t pos) const
    {
        BURDA_LC_LOCK_SHARED

        return T::operator[](pos);
    }

    BURDA_LC_IMPLEMENT_SHARED_ACCESS(data)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(front)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(back)

    /// capacity
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(empty)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(size)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(max_size)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(resize)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(capacity)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(reserve)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(shrink_to_fit)

    /// modifiers
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(clear)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(insert)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(insert_or_assign)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(emplace)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(emplace_hint)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(try_emplace)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(erase)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(push_front)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(emplace_front)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(pop_front)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(push_back)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(emplace_back)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(pop_back)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(swap)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(merge)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(extract)

    /// list operations
    // TODO

    /// lookup
    // TODO
};
}
}
