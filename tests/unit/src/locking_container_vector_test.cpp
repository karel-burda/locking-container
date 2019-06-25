#include <chrono>
#include <cstdlib>
#include <future>
#include <thread>
#include <vector>

#include "locking_container_test.hpp"

namespace burda
{
namespace stl
{
namespace testing
{
TEST(ts_container_vector_test, methods)
{
    {
        testing::create_and_test_container_methods<int, std::vector<int>>();
    }

    {
        struct Foo
        {
            bool operator==(const Foo & rhs) const
            {
                return val == rhs.val;
            }

            const int val = 0;
        };

        testing::create_and_test_container_methods<Foo, std::vector<Foo>>();
    }
}

TEST(ts_container_vector_test, thread_safety)
{
    constexpr const long number_of_seconds = 60;

    std::clog << "Warning: this test will take approximately " << number_of_seconds << " seconds" << std::endl;

    locking_container<std::vector<unsigned char>> container(4096, 1);
    std::atomic<bool> run{true};

    const auto inserter = std::async(std::launch::async, [&]()
    {
        while (run)
        {
            container.push_back_write_lock(1);

            std::this_thread::yield();
        }
    });

    const auto eraser = std::async(std::launch::async, [&]()
    {
        while (run)
        {
            container.write_lock([&container]()
            {
                container.clear();
                container.shrink_to_fit();
            });

            std::this_thread::yield();
        }
    });

    const auto reader = std::async(std::launch::async, [&]()
    {
        while (run)
        {
            container.read_lock([&container]()
            {
                if (!container.empty())
                {
                    ASSERT_EQ(container.front(), 1);
                }
            });

            std::this_thread::yield();
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds{number_of_seconds});

    run = false;

    inserter.wait();
    eraser.wait();
    reader.wait();
}
}
}
}