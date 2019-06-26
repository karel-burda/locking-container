#include <deque>
#include <forward_list>
#include <iterator>
#include <stack>
#include <type_traits>
#include <vector>
#include <queue>

#include <gtest/gtest.h>

#include <locking_container/locking_container.hpp>

namespace burda
{
namespace stl
{
namespace testing
{
template <typename T, typename U>
void test_capacity(const T & instance)
{
    ASSERT_EQ(instance.empty(), instance.empty_read_lock());

    if (!std::is_base_of<std::forward_list<U>, T>::value)
    {
        ASSERT_EQ(instance.size(), instance.size_read_lock());
    }
}

template <typename T, typename U>
void test_iterators(const T & instance)
{
    {
        ASSERT_EQ(instance.begin(), instance.begin_no_lock());
        ASSERT_EQ(instance.cbegin(), instance.cbegin_no_lock());
        ASSERT_EQ(instance.end(), instance.end_no_lock());
        ASSERT_EQ(instance.cend(), instance.cend_no_lock());
    }

    {
        ASSERT_EQ(std::begin(instance), instance.begin());
        ASSERT_EQ(std::cbegin(instance), instance.cbegin());
        ASSERT_EQ(std::end(instance), instance.end());
        ASSERT_EQ(std::begin(instance), std::end(instance));
    }
}

template <typename T, typename U>
void test_element_access(const T & instance)
{
    // TODO: more types
    if constexpr (std::is_base_of<std::array<U, 0>, T>::value ||
                  std::is_base_of<std::vector<U>, T>::value)
    {
        ASSERT_EQ(instance.data(), instance.data_read_lock());
    }

    if constexpr (std::is_base_of<std::array<U, 0>, T>::value ||
                  std::is_base_of<std::vector<U>, T>::value ||
                  std::is_base_of<std::deque<U>, T>::value)
    {
        EXPECT_ANY_THROW(instance.at(0));
        EXPECT_ANY_THROW(instance.at_read_lock(0));
    }
}

template <typename T, typename U>
void test_modifiers(T & instance)
{
    if constexpr (!std::is_base_of<std::array<U, 0>, T>::value &&
                  !std::is_base_of<std::stack<U>, T>::value &&
                  !std::is_base_of<std::queue<U>, T>::value &&
                  !std::is_base_of<std::priority_queue<U>, T>::value)
    {
        EXPECT_NO_THROW(instance.clear_write_lock());
    }
}

template <typename T>
void test_operators(const T & instance, const size_t size)
{
    ASSERT_EQ(instance[0], instance.at(0));
    ASSERT_EQ(instance[size-1], instance.at(size-1));
}

template <typename T>
void test_callbacks(const T & instance)
{
    {
        bool callback_called = false;
        instance.read_lock([&]() { callback_called = true; });
        EXPECT_TRUE(callback_called);
    }

    {
        bool callback_called = false;
        instance.write_lock([&]() { callback_called = true; });
        EXPECT_TRUE(callback_called);
    }
}

template <typename T, typename U>
void create_and_test_container_methods()
{
    using type_base = T;
    using type_container = U;

    {
        locking_container<type_container> empty_instance;

        test_capacity<decltype(empty_instance), type_base>(empty_instance);
        test_iterators<decltype(empty_instance), type_base>(empty_instance);
        test_element_access<decltype(empty_instance), type_base>(empty_instance);
        test_modifiers<decltype(empty_instance), type_base>(empty_instance);
        test_callbacks(empty_instance);
    }

    {
        constexpr const size_t size = 32;
        locking_container<type_container> dummy_instance(size);

        test_operators(dummy_instance, size);
    }
}
}
}
}
