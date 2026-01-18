#pragma once

#include <cyqlone/config.hpp>
#include <batmat/assume.hpp>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <memory>
#include <type_traits>

#ifndef CYQLONE_SANITY_CHECKS_BARRIER
#ifndef NDEBUG
#define CYQLONE_SANITY_CHECKS_BARRIER 1
#else
#define CYQLONE_SANITY_CHECKS_BARRIER 0
#endif
#endif

namespace cyqlone {

struct EmptyCompletion {
    void operator()() const noexcept {}
};

/**
 * Fairly vanilla combining tree barrier. It is inspired by GCC 15.2's __tree_barrier, with some
 * important API differences:
 *  - Every thread has a unique thread ID in [0, expected-1]. This eliminates the need for hashing
 *    the pthread thread IDs and for the inner search loop to find free slots in the tree.
 *  - Wait tries to spin for a given number of iterations before falling back to a futex-based
 *    atomic wait.
 *  - The barrier phase is exposed to the user.
 */
template <typename CompletionFn = EmptyCompletion, class PhaseType = uint32_t>
class TreeBarrier {
  public:
    enum class BarrierPhase : PhaseType {};
    class arrival_token {
        BarrierPhase phase;

      public:
        explicit arrival_token(BarrierPhase phase) : phase{phase} {}
        arrival_token(const arrival_token &phase)            = delete;
        arrival_token(arrival_token &&phase)                 = default;
        arrival_token &operator=(const arrival_token &phase) = delete;
        arrival_token &operator=(arrival_token &&phase)      = default;
        BarrierPhase get() const noexcept { return phase; }
    };

  private:
    static constexpr size_t cache_line_size = 64;
    struct Storage {
        alignas(cache_line_size) std::array<std::byte, cache_line_size> payload;
        template <class T>
        void store(T t) noexcept {
            static_assert(sizeof(T) <= sizeof(payload));
            static_assert(std::is_trivially_copyable_v<T>);
            std::memcpy(payload.data(), &t, sizeof(T));
        }
        template <class T>
        T load() const noexcept {
            static_assert(sizeof(T) <= sizeof(payload));
            static_assert(std::is_trivially_copyable_v<T>);
            T t;
            std::memcpy(&t, payload.data(), sizeof(T));
            return t;
        }
    };
    struct alignas(cache_line_size) State {
        using atomic_byte = std::atomic<unsigned char>;
        using atomic_word = std::atomic<uint32_t>;
        static constexpr bool only_word_lock_free =
            atomic_word::is_always_lock_free && !atomic_byte::is_always_lock_free;
        using ticket_t = std::conditional_t<only_word_lock_free, atomic_word, atomic_byte>;
        static constexpr size_t num_levels = cache_line_size / sizeof(ticket_t);
        std::array<ticket_t, num_levels> tickets{};
    };
    using ticket_value_type = typename State::ticket_t::value_type;

    uint32_t expected;
    std::unique_ptr<State[]> state;
    std::unique_ptr<Storage[]> storage;
    Storage broadcast_storage;
    [[no_unique_address]] CompletionFn completion;
    alignas(cache_line_size) std::atomic<BarrierPhase> phase{};

#if CYQLONE_SANITY_CHECKS_BARRIER
    State::ticket_t &get_local_phase(uint32_t thread_id) noexcept {
        return state[thread_id >> 1].tickets[State::num_levels - 1 - (thread_id & 1)];
    }
    State::ticket_t &get_local_line(uint32_t thread_id) noexcept {
        return state[thread_id >> 1].tickets[State::num_levels - 3 - (thread_id & 1)];
    }
    void sanity_check_arrival(uint32_t thread_id, BarrierPhase cur_phase) noexcept {
        if (get_local_phase(thread_id).fetch_add(1, std::memory_order_relaxed) !=
            static_cast<ticket_value_type>(cur_phase))
            BATMAT_ASSERT(!"This thread has already arrived in this phase");
    }
#endif

    bool arrive_impl(BarrierPhase old_phase, uint32_t thread_id) {
        static constexpr auto acq_rel = std::memory_order_acq_rel;
        const auto first_of_one       = static_cast<ticket_value_type>(old_phase),
                   second_of_two      = static_cast<ticket_value_type>(2 * first_of_one + 1);

        uint32_t level_size = expected; // Total sum in this level of the tree
        for (size_t level = 0;; ++level) {
            if (level_size <= 1)
                return true;
            BATMAT_ASSUME(level < 32);
            thread_id >>= 1;
            auto &ticket            = state[thread_id].tickets[level];
            const uint32_t end_node = (level_size + 1) >> 1; // Two threads per node
            const bool last_odd     = thread_id + 1 == end_node && (level_size & 1) == 1;
            const auto target       = last_odd ? first_of_one : second_of_two;
            const auto old_value    = ticket.fetch_add(1, acq_rel);
            if (old_value != target)
                return false;
            level_size = end_node;
        }
    }

    //                   l=0  l=1  l=2
    // storage[0]   t=0   1    3    10
    // storage[1]   t=1   2
    // storage[2]   t=2   3    7
    // storage[3]   t=3   4

    template <class T, class F>
    bool arrive_impl(BarrierPhase old_phase, uint32_t thread_id, T value, F reduce) {
        static constexpr auto acq_rel = std::memory_order_acq_rel;
        const auto first_of_one       = static_cast<ticket_value_type>(old_phase),
                   second_of_two      = static_cast<ticket_value_type>(2 * first_of_one + 1);

        uint32_t level_size = expected; // Total sum in this level of the tree
        for (size_t level = 0;; ++level) {
            if (level_size <= 1) {
                broadcast_storage.store(value);
                return true;
            }
            BATMAT_ASSUME(level < 32);
            auto offset = size_t{1} << level;
            auto write  = thread_id << level;
            storage[write].store(value);
            thread_id >>= 1;
            auto &ticket            = state[thread_id].tickets[level];
            const uint32_t end_node = (level_size + 1) >> 1; // Two threads per node
            const bool last_odd     = thread_id + 1 == end_node && (level_size & 1) == 1;
            const auto target       = last_odd ? first_of_one : second_of_two;
            const auto old_value    = ticket.fetch_add(1, acq_rel);
            if (old_value != target)
                return false;
            if (!last_odd)
                value = reduce(storage[write & ~offset].template load<T>(),
                               storage[write | +offset].template load<T>());
            level_size = end_node;
        }
    }

  public:
    static constexpr uint32_t max() {
#if CYQLONE_SANITY_CHECKS_BARRIER // Leave space for local phases for sanity checks
        constexpr static uint32_t num_levels = State::num_levels - 4;
#else
        constexpr static uint32_t num_levels = State::num_levels;
#endif
        return num_levels > 31 ? 0xFFFFFFFF : uint32_t{1} << num_levels;
    }

    TreeBarrier(uint32_t expected, CompletionFn completion)
        : expected(expected), completion(std::move(completion)),
          phase(static_cast<BarrierPhase>(0)) {
        BATMAT_ASSERT(expected <= max());
        const size_t leaf_count = (expected + 1) >> 1;
        state                   = std::make_unique<State[]>(leaf_count);
        storage                 = std::make_unique<Storage[]>(expected);
    }
    TreeBarrier(const TreeBarrier &)            = delete;
    TreeBarrier(TreeBarrier &&)                 = default;
    TreeBarrier &operator=(const TreeBarrier &) = delete;
    TreeBarrier &operator=(TreeBarrier &&)      = default;

    [[nodiscard]] arrival_token arrive(uint32_t thread_id) {
        BATMAT_ASSUME(thread_id < expected);
        const auto cur_phase = phase.load(std::memory_order_relaxed);
#if CYQLONE_SANITY_CHECKS_BARRIER
        sanity_check_arrival(thread_id, cur_phase);
#endif
        if (arrive_impl(cur_phase, thread_id)) {
            completion();
            auto next_phase = static_cast<BarrierPhase>(static_cast<PhaseType>(cur_phase) + 1);
            phase.store(next_phase, std::memory_order_release);
            phase.notify_all();
        }
        return arrival_token{cur_phase};
    }

    [[nodiscard]] arrival_token arrive(uint32_t thread_id, [[maybe_unused]] int line) {
        BATMAT_ASSUME(thread_id < expected);
        const auto cur_phase = phase.load(std::memory_order_relaxed);
#if CYQLONE_SANITY_CHECKS_BARRIER
        sanity_check_arrival(thread_id, cur_phase);
#endif
#if CYQLONE_SANITY_CHECKS_BARRIER
        get_local_line(thread_id).store(static_cast<ticket_value_type>(line),
                                        std::memory_order_relaxed);
#endif
        if (arrive_impl(cur_phase, thread_id)) {
#if CYQLONE_SANITY_CHECKS_BARRIER
            for (uint32_t i = 0; i < expected; ++i)
                BATMAT_ASSERT(get_local_line(i).load(std::memory_order_relaxed) ==
                              static_cast<ticket_value_type>(line));
#endif
            completion();
            auto next_phase = static_cast<BarrierPhase>(static_cast<PhaseType>(cur_phase) + 1);
            phase.store(next_phase, std::memory_order_release);
            phase.notify_all();
        }
        return arrival_token{cur_phase};
    }

    [[nodiscard]] BarrierPhase current_phase() const {
        return phase.load(std::memory_order_relaxed);
    }

    uint32_t spin_count = 1000; // approx. 2-3 cycles/iteration (Haswell, according to llvm-mca)

    /// Check if @ref wait() may block. If it returns false, the caller can call @ref wait()
    /// and it will return immediately without spinning or sleeping. This is useful if the caller
    /// has other non-critical work to do while waiting for other threads.
    /// Users should still call @ref wait() before arriving again.
    /// @note This function does not impose any memory ordering, so even when it returns false,
    ///       changes made before the arrival of other threads may not be visible yet. In contrast,
    ///       @ref wait() does ensure proper synchronization.
    bool wait_may_block(const arrival_token &token) const noexcept {
        return phase.load(std::memory_order_relaxed) == token.get();
    }

    void wait(arrival_token &&token) const {
        const auto old_phase = token.get();
        // barring overflow, we have that current_phase >= old_phase
        if (phase.load(std::memory_order_acquire) != old_phase) [[likely]]
            return;
        // Spin before calling wait
        for (auto spin = this->spin_count; spin-- > 0;)
            if (phase.load(std::memory_order_acquire) != old_phase) [[unlikely]]
                return;
        phase.wait(old_phase, std::memory_order_acquire);
    }

    void arrive_and_wait(uint32_t thread_id) { wait(arrive(thread_id)); }
    void arrive_and_wait(uint32_t thread_id, int line) { wait(arrive(thread_id, line)); }

    /// Combining tree reduction across all threads. Deterministic for a given number of threads.
    template <class T, class F>
    [[nodiscard]] T reduce(uint32_t thread_id, T x, F reduce) {
        BATMAT_ASSUME(thread_id < expected);
        const auto cur_phase = phase.load(std::memory_order_relaxed);
#if CYQLONE_SANITY_CHECKS_BARRIER
        sanity_check_arrival(thread_id, cur_phase);
#endif
        if (arrive_impl(cur_phase, thread_id, std::move(x), std::move(reduce))) {
            completion();
            auto next_phase = static_cast<BarrierPhase>(static_cast<PhaseType>(cur_phase) + 1);
            phase.store(next_phase, std::memory_order_release);
            phase.notify_all();
        }
        wait(arrival_token{cur_phase});
        return broadcast_storage.template load<T>();
    }

    template <class T>
    [[nodiscard]] T broadcast(uint32_t thread_id, T x, uint32_t src = 0) {
        // TODO: optimized implementation without as many copies
        if (thread_id == src)
            return reduce(thread_id, std::move(x), [](const T &a, const T &) { return a; });
        else
            return reduce(thread_id, T{}, [](const T &, const T &b) { return b; });
    }
};

} // namespace cyqlone
