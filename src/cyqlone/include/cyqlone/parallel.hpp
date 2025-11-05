#pragma once

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
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace CYQLONE_NAMESPACE::parallel {

struct SharedContext;
template <class SC = SharedContext>
struct Context;

struct SharedContext {
#if GUANAQO_WITH_TRACING
    struct completion_type {
        void operator()() const noexcept {
            auto trace = guanaqo::trace_logger.trace("barrier-complete", 0);
            trace.log  = nullptr;
        }
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
    std::vector<std::byte> workspace = std::vector<std::byte>(static_cast<size_t>(num_thr) * 64);
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

template <class SC>
struct Context {
    using shared_context_type = SC;
#if GUANAQO_WITH_TRACING
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
    std::optional<arrival_token> *token = nullptr;

    [[nodiscard]] bool is_master() const { return index == 0; }

    /// Low-level: token must be awaited before any other call to arrive.
    arrival_token arrive() {
        wait();
#if GUANAQO_WITH_TRACING
        auto trace = guanaqo::trace_logger.trace("barrier-arrive", index);
        return {shared.barrier.arrive(static_cast<uint32_t>(index)), std::move(trace)};
#else
        return shared.barrier.arrive(static_cast<uint32_t>(index));
#endif
    }
    /// Low-level: await a token returned by arrive().
    void wait(arrival_token &&token) {
#if GUANAQO_WITH_TRACING
        auto trace = std::move(token.trace);
        shared.barrier.wait(std::move(token.token));
#else
        shared.barrier.wait(std::move(token));
#endif
    }

    // Waits for the previous phase to succeed if we already arrived
    // without waiting, and then arrives at the barrier again, saving
    // the arrival token to the given optional.
    // Later calls to arrive() will first await this optional if it
    // contains a token. This means that the optional should remain
    // alive.
    void arrive(std::optional<arrival_token> &token) {
        wait();
        this->token = &token;
#if GUANAQO_WITH_TRACING
        auto trace = guanaqo::trace_logger.trace("barrier-arrive", index);
        token.emplace(shared.barrier.arrive(static_cast<uint32_t>(index)), std::move(trace));
#else
        token = shared.barrier.arrive(static_cast<uint32_t>(index));
#endif
    }
    void arrive_and_wait() {
        wait();
        GUANAQO_TRACE("barrier-arrive-and-wait", index);
        shared.barrier.arrive_and_wait(static_cast<uint32_t>(index));
    }
    void arrive_and_wait(int line) {
        wait();
        GUANAQO_TRACE("barrier-arrive-and-wait", index);
        shared.barrier.arrive_and_wait(static_cast<uint32_t>(index), line);
    }
    bool wait(std::optional<arrival_token> &token) {
        if (!token)
            return false;
        wait(*std::exchange(token, std::nullopt));
        return true;
    }
    bool wait() {
        if (!token)
            return false;
        return wait(*std::exchange(token, nullptr));
    }

    template <class T>
    T *get_workspace_ptr(index_t idx) {
        const size_t slot_size = shared.workspace.size() / num_thr;
        BATMAT_ASSERT(sizeof(T) <= slot_size);
        const size_t offset = slot_size * static_cast<size_t>(idx);
        void *dest          = shared.workspace.data() + offset;
        size_t space        = slot_size;
        bool ok             = std::align(alignof(T), sizeof(T), dest, space);
        BATMAT_ASSERT(ok);
        return std::launder(reinterpret_cast<T *>(dest));
    }

    template <class T>
    T broadcast(T x, index_t src = 0) {
        void *dest   = shared.workspace.data();
        size_t space = shared.workspace.size();
        bool ok      = std::align(alignof(T), sizeof(T), dest, space);
        BATMAT_ASSERT(ok);
        if (index == src)
            new (dest) T(std::move(x));
        arrive_and_wait();
        x = *std::launder(reinterpret_cast<T *>(dest));
        arrive_and_wait(); // Ensure that the workspace is not used before everyone is done
        return x;
    }

    template <class F, class... Args>
    auto call_broadcast(F &&f, Args &&...args) -> std::invoke_result_t<F, Args...> {
        using T      = std::invoke_result_t<F, Args...>;
        void *dest   = shared.workspace.data();
        size_t space = shared.workspace.size();
        bool ok      = std::align(alignof(T), sizeof(T), dest, space);
        BATMAT_ASSERT(ok);
        if (is_master())
            new (dest) T(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
        arrive_and_wait();
        T r = *std::launder(reinterpret_cast<T *>(dest));
        arrive_and_wait(); // Ensure that the workspace is not used before everyone is done
        return r;
    }

    template <class T, class F>
    T reduce(T x, T init, F func) {
        auto dest = get_workspace_ptr<T>(index);
        new (dest) T(std::move(x));
        arrive_and_wait();
        // TODO: use a tree reduction
        for (index_t i = 0; i < num_thr; ++i)
            init = func(init, *get_workspace_ptr<T>(i));
        arrive_and_wait(); // Ensure that the workspace is not used before everyone is done
        return init;
    }

    template <class T>
    T reduce(T x, T init) {
        return reduce(std::move(x), std::move(init), std::plus<>{});
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

template <class T, class SC>
struct SharedResult {
    using shared_context_type = SC;
    using context_type        = Context<shared_context_type>;
    using arrival_token       = typename context_type::arrival_token;

    struct alignas(64) Result {
        T value;
        std::optional<arrival_token> token;
    };
    std::unique_ptr<Result[]> results;

    SharedResult(shared_context_type &shared)
        : results(std::make_unique_for_overwrite<Result[]>(shared.num_thr)) {}

    void wait(context_type &ctx) { ctx.wait(results[ctx.index].token); }
};

template <class SC>
struct SharedResult<void, SC> {
    using shared_context_type = SC;
    using context_type        = Context<shared_context_type>;
    using arrival_token       = typename context_type::arrival_token;

    struct Result {
        std::optional<arrival_token> token;
    };
    std::unique_ptr<Result[]> results;

    SharedResult(shared_context_type &shared)
        : results(std::make_unique_for_overwrite<Result[]>(shared.num_thr)) {}

    void wait(context_type &ctx) { ctx.wait(results[ctx.index].token); }
};

} // namespace CYQLONE_NAMESPACE::parallel
