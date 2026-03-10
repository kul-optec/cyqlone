#pragma once

/// @file
/// Parallel execution context and synchronization primitives.
/// @ingroup topic-parallelization

#include <cyqlone/barrier.hpp>
#include <cyqlone/config.hpp>
#include <batmat/config.hpp>
#if BATMAT_WITH_OPENMP
#include <batmat/openmp.h>
#else
#include <batmat/thread-pool.hpp>
#endif
#include <guanaqo/trace.hpp>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace cyqlone::parallel {

struct SharedContext;
template <class SC = SharedContext>
struct Context;

/// Abstraction for a parallel execution context: a set of threads that can synchronize and
/// communicate with each other using barriers.
/// @see Context
/// @ingroup topic-parallelization
struct SharedContext {
#if GUANAQO_WITH_TRACING
    struct completion_type {
        void operator()() const noexcept { GUANAQO_TRACE_INSTANT("barrier-complete", 0); }
    };
#else
    using completion_type = EmptyCompletion;
#endif
    using barrier_type = TreeBarrier<completion_type, uint16_t>;
    const index_t num_thr;
    barrier_type barrier{static_cast<uint32_t>(num_thr), {}};
#if !BATMAT_WITH_OPENMP
    batmat::thread_pool thread_pool{static_cast<size_t>(num_thr)};
#endif
    /// Execute the given function in parallel on all threads, blocking until completion.
    /// The function will be called with a @ref Context that contains the thread index, and that can
    /// be used to synchronize and communicate between threads.
    template <class F>
    void run(F &&);

    /// Configure the barrier spin count used in parallel synchronization before falling back to a
    /// futex wait.
    uint32_t set_barrier_spin_count(uint32_t spin_count) {
        static_assert(std::is_same_v<decltype(barrier.spin_count), decltype(spin_count)>);
        return std::exchange(barrier.spin_count, spin_count);
    }
};

/// Thread context for parallel execution. Each thread has a unique thread index, and can
/// synchronize and communicate with other threads in the same shared context.
/// @see SharedContext
/// @ingroup topic-parallelization
template <class SC>
struct Context {
    using shared_context_type = SC;
#if GUANAQO_WITH_TRACING && !GUANAQO_WITH_PERFETTO
    struct arrival_token {
        using token_t = typename shared_context_type::barrier_type::arrival_token;
        token_t token;
        guanaqo::TraceLogger::ScopedLog trace;
    };
#else
    using arrival_token = typename shared_context_type::barrier_type::arrival_token;
#endif

    shared_context_type &shared;
    const index_t index, num_thr = shared.num_thr;

    friend constexpr bool operator==(const Context &a, const Context &b) {
        return &a.shared == &b.shared && a.index == b.index;
    }

    /// Check if this thread is the master thread (thread index 0).
    /// Useful for determining which thread should perform operations like printing to the console,
    /// which should be done by a single thread and does not require synchronization.
    [[nodiscard]] bool is_master() const { return index == 0; }

    /// Arrive at the barrier and obtain a token that can be used to wait for completion of the
    /// current barrier phase.
    /// @note    Token must be awaited before any other call to arrive.
    [[nodiscard]] arrival_token arrive() {
#if GUANAQO_WITH_TRACING && !GUANAQO_WITH_PERFETTO
        auto trace = guanaqo::get_trace_logger().trace("barrier-arrive", index);
        return {shared.barrier.arrive(static_cast<uint32_t>(index)), std::move(trace)};
#else
        return shared.barrier.arrive(static_cast<uint32_t>(index));
#endif
    }
    /// Await a token returned by @ref arrive(), waiting for the barrier phase to complete.
    void wait(arrival_token &&token) {
#if GUANAQO_WITH_TRACING && !GUANAQO_WITH_PERFETTO
        auto trace = std::move(token.trace);
        shared.barrier.wait(std::move(token.token));
#else
        shared.barrier.wait(std::move(token));
#endif
    }

    /// Arrive at the barrier and wait for the barrier phase to complete. This is a convenience
    /// wrapper around @ref arrive() and @ref wait() for the common case where the thread does not
    /// have other work to do while waiting.
    void arrive_and_wait() {
#if !GUANAQO_WITH_PERFETTO
        GUANAQO_TRACE("barrier-arrive-and-wait", index);
#endif
        shared.barrier.arrive_and_wait(static_cast<uint32_t>(index));
    }
    /// Debug version of @ref arrive_and_wait() that performs a sanity check to ensure that all
    /// threads are arriving at the same line of code. The @p line parameter should be the same
    /// for all threads arriving at the same barrier. It is only verified in debug builds, and is
    /// equivalent to @ref arrive_and_wait() in release builds.
    void arrive_and_wait(int line) {
#if !GUANAQO_WITH_PERFETTO
        GUANAQO_TRACE("barrier-arrive-and-wait", index);
#endif
        shared.barrier.arrive_and_wait(static_cast<uint32_t>(index), line);
    }

    /// Broadcast a value @p x from the thread with index @p src to all threads.
    template <class T>
    T broadcast(T x, index_t src = 0) {
        return shared.barrier.broadcast(static_cast<uint32_t>(index), std::move(x),
                                        static_cast<uint32_t>(src));
    }

    /// Call a function @p f with the given @p args on a single thread and broadcast the return
    /// value to all threads.
    template <class F, class... Args>
    auto call_broadcast(F &&f, Args &&...args) -> std::invoke_result_t<F, Args...> {
        using T = std::invoke_result_t<F, Args...>;
        // TODO: implement with a relaxed atomic that gets reset during the completion handler,
        //       so only the first thread that arrives will execute the function, rather than always
        //       executing on the master thread.
        if (is_master())
            return broadcast(std::invoke(std::forward<F>(f), std::forward<Args>(args)...), 0);
        else
            return broadcast(T{}, 0);
    }

    /// Perform a reduction of @p x across all threads using the given binary function @p func.
    /// Returns a token that can be used to wait for the reduction to complete and obtain the
    /// reduced value.
    template <class T, class F>
    [[nodiscard]] auto arrive_reduce(T x, F func) {
        return shared.barrier.arrive_reduce(static_cast<uint32_t>(index), std::move(x),
                                            std::move(func));
    }

    /// Wait for the reduction initiated by @ref arrive_reduce() to complete and obtain the reduced
    /// value.
    template <class T>
    T wait_reduce(shared_context_type::barrier_type::template arrival_token_typed<T> &&token) {
        return shared.barrier.wait_reduce(std::move(token));
    }

    /// Perform a reduction of @p x across all threads using the given binary function @p func, and
    /// wait for the result.
    template <class T, class F>
    T reduce(T x, F func) {
        return shared.barrier.reduce(static_cast<uint32_t>(index), std::move(x), std::move(func));
    }

    /// Reduction with `std::plus`, i.e., summation across all threads.
    /// @see reduce(T,F)
    template <class T>
    T reduce(T x) {
        return reduce(std::move(x), std::plus<>{});
    }

    /// Wait for all threads to reach this point, then run the given function on a single thread
    /// before releasing all threads again. Changes by all threads are visible during the call to
    /// @p f and changes made by @p f are visible to all threads after this function returns.
    template <class F>
    void run_single_sync(F &&f) {
        shared.barrier.arrive_and_wait_with_completion(static_cast<uint32_t>(index),
                                                       std::forward<F>(f));
    }
};

template <class F>
void SharedContext::run(F &&f) {
#if !BATMAT_WITH_OPENMP
    thread_pool.sync_run_n(num_thr, [this, &f](index_t i, index_t) {
        Context<SharedContext> ctx{.shared = *this, .index = i};
        f(ctx);
    });
#else
    BATMAT_OMP(parallel for num_threads(num_thr))
    for (index_t i = 0; i < num_thr; ++i) {
        Context<SharedContext> ctx{.shared = *this, .index = i};
        f(ctx);
    }
#endif
}

} // namespace cyqlone::parallel
