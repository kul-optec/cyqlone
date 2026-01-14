#include <cyqlone/barrier.hpp>
#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <version>

class BarrierTest : public ::testing::TestWithParam<uint32_t> {};

TEST_P(BarrierTest, stress) {
    const uint32_t num_workers = GetParam();
    const uint32_t iterations  = num_workers < 10 ? 1'000'000 : 1'000;

    using Barrier = cyqlone::TreeBarrier<>;
    ASSERT_GE(Barrier::max(), num_workers + 1);
    Barrier barrier(num_workers + 1, {});

    std::atomic<uint32_t> counter{0};
    std::vector<std::jthread> threads;
    threads.reserve(num_workers);

    // Worker threads
    for (uint32_t i = 0; i < num_workers; ++i) {
        threads.emplace_back([&, i] {
            uint32_t success = 0;
            for (uint32_t iter = 0; iter < iterations; ++iter) {
                uint32_t val = counter.load(std::memory_order_relaxed);
                success += (val == iter || val == iter + 1);
                success += decltype(barrier)::BarrierPhase{iter} == barrier.current_phase();
                barrier.arrive_and_wait(i + 1); // worker IDs start at 1
            }
            EXPECT_EQ(success, 2 * iterations) << i;
        });
    }

    // Counter thread (thread_id = 0)
    std::jthread counter_thread{[&] {
        for (uint32_t iter = 0; iter < iterations; ++iter) {
            counter.fetch_add(1, std::memory_order_relaxed);
            barrier.arrive_and_wait(0);
        }
    }};
}

#if __cpp_lib_barrier >= 201907L
#include <barrier>

TEST_P(BarrierTest, std) {
    const uint32_t num_workers = GetParam();
    const uint32_t iterations  = num_workers < 10 ? 1'000'000 : 1'000;

    using Barrier = std::barrier<>;
    ASSERT_GE(Barrier::max(), num_workers + 1);
    Barrier barrier(num_workers + 1, {});

    std::atomic<uint32_t> counter{0};
    std::vector<std::jthread> threads;
    threads.reserve(num_workers);

    // Worker threads
    for (uint32_t i = 0; i < num_workers; ++i) {
        threads.emplace_back([&, i] {
            uint32_t success = 0;
            for (uint32_t iter = 0; iter < iterations; ++iter) {
                uint32_t val = counter.load(std::memory_order_relaxed);
                success += (val == iter || val == iter + 1);
                barrier.arrive_and_wait();
            }
            EXPECT_EQ(success, iterations) << i;
        });
    }

    // Counter thread (thread_id = 0)
    std::jthread counter_thread{[&] {
        for (uint32_t iter = 0; iter < iterations; ++iter) {
            counter.fetch_add(1, std::memory_order_relaxed);
            barrier.arrive_and_wait();
        }
    }};
}
#endif

TEST_P(BarrierTest, reduce) {
    const uint32_t num_workers = GetParam();
    const uint32_t iterations  = num_workers < 10 ? 1'000'000 : 1'000;

    using Barrier = cyqlone::TreeBarrier<>;
    ASSERT_GE(Barrier::max(), num_workers);
    Barrier barrier(num_workers, {});

    std::vector<std::jthread> threads;
    threads.reserve(num_workers);

    for (uint32_t i = 0; i < num_workers; ++i) {
        threads.emplace_back([&, i] {
            uint32_t success = 0;
            for (uint32_t iter = 0; iter < iterations; ++iter) {
                auto res = barrier.reduce(i, i + 1, std::plus{});
                success += res == num_workers * (num_workers + 1) / 2;
            }
            EXPECT_EQ(success, iterations) << i;
        });
    }
}

// Instantiate the test with different numbers of worker threads
INSTANTIATE_TEST_SUITE_P(BarrierTest, BarrierTest,
                         ::testing::Values(1, 2, 3, 4, 5, 6, 7, 8, 9, 254, 255, 256, 257));
