#pragma once

#include "locking_container/locking_container_basic.hpp"

namespace burda
{
namespace stl
{
/**
 * Implements thread-safe variants (as new methods) of all containers' methods, list available at: https://en.cppreference.com/w/cpp/container
 * @see "locking_container_basic"
 */
template <typename T>
class locking_container : public locking_container_basic<T>
{
public:
    using locking_container_basic<T>::locking_container_basic;

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

    /// iterators
    BURDA_LC_IMPLEMENT_ITERATOR(begin)
    BURDA_LC_IMPLEMENT_CITERATOR(cbegin)
    BURDA_LC_IMPLEMENT_ITERATOR(end)
    BURDA_LC_IMPLEMENT_CITERATOR(cend)
    BURDA_LC_IMPLEMENT_ITERATOR(rbegin)
    BURDA_LC_IMPLEMENT_CITERATOR(crbegin)
    BURDA_LC_IMPLEMENT_ITERATOR(rend)
    BURDA_LC_IMPLEMENT_CITERATOR(crend)

    /// modifiers (alphabetically)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(clear)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(emplace)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(erase)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(insert)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(insert_or_assign)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(push_back)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(pop_back)

    /// element access (alphabetically)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(at)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(back)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(data)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(front)

    /// capacity (alphabetically)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(capacity)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(empty)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(find)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(max_size)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(reserve)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(resize)
    BURDA_LC_IMPLEMENT_SHARED_ACCESS(size)
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(shrink_to_fit)

    /// other
    BURDA_LC_IMPLEMENT_EXCLUSIVE_ACCESS(assign)
};
}
}
