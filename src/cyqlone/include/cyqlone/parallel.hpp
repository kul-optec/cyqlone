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
};

/*

  The following is not allowed when using barriers:

        barrier.arrive_and_wait(t);
        compute_local_sums(X);
        tok_X = barrier.arrive(t);
        compute_local_sums(Y);
        tok_Y = barrier.arrive(t);         // We should have waited before arriving again
        barrier.wait(tok_X);
        barrier.wait(tok_Y);
        compute_global_sum(X);
        compute_global_sum(Y);

  The following is optimal (?):

        barrier.arrive_and_wait(t);
    local sums X:
        compute_local_sums(X);
        tok_X = barrier.arrive(t);
    local sums Y:
        compute_local_sums(Y);
        barrier.wait(tok_X);               // This needs to be taken care of by the context
        tok_Y = barrier.arrive(t);
    global sums X:
        compute_global_sum(X);             // We need to detect that waiting is not needed
    global sums Y:
        barrier.wait(tok_Y);
        compute_global_sum(Y);

*/

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

    [[nodiscard]] bool is_master() const { return index == 0; }

    /// Low-level: token must be awaited before any other call to arrive.
    arrival_token arrive() {
#if GUANAQO_WITH_TRACING && !GUANAQO_WITH_PERFETTO
        auto trace = guanaqo::get_trace_logger().trace("barrier-arrive", index);
        return {shared.barrier.arrive(static_cast<uint32_t>(index)), std::move(trace)};
#else
        return shared.barrier.arrive(static_cast<uint32_t>(index));
#endif
    }
    /// Low-level: await a token returned by arrive().
    void wait(arrival_token &&token) {
#if GUANAQO_WITH_TRACING && !GUANAQO_WITH_PERFETTO
        auto trace = std::move(token.trace);
        shared.barrier.wait(std::move(token.token));
#else
        shared.barrier.wait(std::move(token));
#endif
    }

    void arrive_and_wait() {
#if !GUANAQO_WITH_PERFETTO
        GUANAQO_TRACE("barrier-arrive-and-wait", index);
#endif
        shared.barrier.arrive_and_wait(static_cast<uint32_t>(index));
    }
    void arrive_and_wait(int line) {
#if !GUANAQO_WITH_PERFETTO
        GUANAQO_TRACE("barrier-arrive-and-wait", index);
#endif
        shared.barrier.arrive_and_wait(static_cast<uint32_t>(index), line);
    }

    template <class T>
    T broadcast(T x, index_t src = 0) {
        return shared.barrier.broadcast(static_cast<uint32_t>(index), std::move(x),
                                        static_cast<uint32_t>(src));
    }

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

    template <class T, class F>
    auto arrive_reduce(T x, F func) {
        return shared.barrier.arrive_reduce(static_cast<uint32_t>(index), std::move(x),
                                            std::move(func));
    }

    template <class T>
    T wait_reduce(shared_context_type::barrier_type::template arrival_token_typed<T> &&token) {
        return shared.barrier.wait_reduce(std::move(token));
    }

    template <class T, class F>
    T reduce(T x, F func) {
        return shared.barrier.reduce(static_cast<uint32_t>(index), std::move(x), std::move(func));
    }

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
