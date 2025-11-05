#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/neumaier.hpp>
#include <guanaqo/tag-invoke.hpp>
#include <span>
#include <vector>
#if CYQLONE_QUADRUPLE_SUM_LINE_SEARCH
#include <stdfloat>
#endif

namespace CYQLONE_NAMESPACE::qpalm {

struct Breakpoint {
    // t = α/δ   <=>   α = t δ
    real_t t, δ;
    [[gnu::always_inline, nodiscard]] real_t α() const { return t * δ; }
};

#if CYQLONE_QUADRUPLE_SUM_LINE_SEARCH
using ABSum_t = std::float128_t;
#elif CYQLONE_COMPENSATE_SUM_LINE_SEARCH
using ABSum_t = NeumaierSum<real_t>;
#else
using ABSum_t = real_t;
#endif

struct ABSums {
    ABSum_t a, b;
};

struct PartitionedBreakpoints {
    std::span<Breakpoint> neg_bp, pos_bp;
};

ABSums partial_sum_negative(PartitionedBreakpoints breakpoints, real_t η = 0, real_t β = 0);

struct BreakpointsResult {
    PartitionedBreakpoints bp;
    ABSums ab_neg;
};

/// Compute the break points t[i] using formula (3.6) in the QPALM paper.
/// @return t, α, δ
template <class Vec>
std::span<Breakpoint>
compute_breakpoints_default(std::vector<Breakpoint> &breakpoints, const Vec &Σ, const Vec &y,
                            const Vec &Ad, const Vec &Ax, const Vec &b_min, const Vec &b_max);

/// Moves any non-finite elements in t to the end of the range, and all negative
/// elements to the front. Returns the negative and positive partitions.
PartitionedBreakpoints partition_breakpoints_default(std::span<Breakpoint> breakpoints);

struct compute_breakpoints_fn {
    template <class Backend>
    using vec_t = typename std::remove_cvref_t<Backend>::ineq_constr_vec_t;

    template <class Backend>
    // The compute_breakpoints type should opt in to the tag to provide a custom
    // implementation.
        requires(guanaqo::tag_invocable<compute_breakpoints_fn, Backend &,
                                        typename Backend::Context &, std::vector<Breakpoint> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &>)
    auto operator()(Backend &backend, typename Backend::Context &ctx,
                    std::vector<Breakpoint> &breakpoints, const vec_t<Backend> &Σ,
                    const vec_t<Backend> &y, const vec_t<Backend> &Ad, const vec_t<Backend> &Ax,
                    const vec_t<Backend> &b_min, const vec_t<Backend> &b_max) const
        noexcept(guanaqo::is_nothrow_tag_invocable_v<
                 compute_breakpoints_fn, Backend &, typename Backend::Context &,
                 std::vector<Breakpoint> &, const vec_t<Backend> &, const vec_t<Backend> &,
                 const vec_t<Backend> &, const vec_t<Backend> &, const vec_t<Backend> &,
                 const vec_t<Backend> &>) {
        return guanaqo::guanaqo_tag_invoke(*this, backend, ctx, breakpoints, Σ, y, Ad, Ax, b_min,
                                           b_max);
    }

    template <class Backend>
    // Fallback implementation for unknown backends
        requires(!guanaqo::tag_invocable<compute_breakpoints_fn, Backend &,
                                         typename Backend::Context &, std::vector<Breakpoint> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &>)
    auto operator()(Backend &, typename Backend::Context &ctx, std::vector<Breakpoint> &breakpoints,
                    const vec_t<Backend> &Σ, const vec_t<Backend> &y, const vec_t<Backend> &Ad,
                    const vec_t<Backend> &Ax, const vec_t<Backend> &b_min,
                    const vec_t<Backend> &b_max) const {
        ctx.arrive_and_Wait();
        return ctx.call_broadcast(
            [&] { return compute_breakpoints_default(breakpoints, Σ, y, Ad, Ax, b_min, b_max); });
    }
} inline constexpr compute_breakpoints;

struct get_partitioned_breakpoints_fn {
    template <class Backend>
    using vec_t = typename std::remove_cvref_t<Backend>::ineq_constr_vec_t;

    template <class Backend>
    // The get_partitioned_breakpoints type should opt in to the tag to provide a custom
    // implementation.
        requires(guanaqo::tag_invocable<get_partitioned_breakpoints_fn, Backend &,
                                        typename Backend::Context &, std::vector<Breakpoint> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &>)
    PartitionedBreakpoints operator()(Backend &backend, typename Backend::Context &ctx,
                                      std::vector<Breakpoint> &breakpoints, const vec_t<Backend> &Σ,
                                      const vec_t<Backend> &y, const vec_t<Backend> &Ad,
                                      const vec_t<Backend> &Ax, const vec_t<Backend> &b_min,
                                      const vec_t<Backend> &b_max) const
        noexcept(guanaqo::is_nothrow_tag_invocable_v<
                 get_partitioned_breakpoints_fn, Backend &, typename Backend::Context &,
                 std::vector<Breakpoint> &, const vec_t<Backend> &, const vec_t<Backend> &,
                 const vec_t<Backend> &, const vec_t<Backend> &, const vec_t<Backend> &,
                 const vec_t<Backend> &>) {
        return guanaqo::guanaqo_tag_invoke(*this, backend, ctx, breakpoints, Σ, y, Ad, Ax, b_min,
                                           b_max);
    }

    template <class Backend>
    // Fallback implementation for unknown backends
        requires(!guanaqo::tag_invocable<get_partitioned_breakpoints_fn, Backend &,
                                         typename Backend::Context &, std::vector<Breakpoint> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &>)
    PartitionedBreakpoints operator()(Backend &backend, typename Backend::Context &ctx,
                                      std::vector<Breakpoint> &breakpoints, const vec_t<Backend> &Σ,
                                      const vec_t<Backend> &y, const vec_t<Backend> &Ad,
                                      const vec_t<Backend> &Ax, const vec_t<Backend> &b_min,
                                      const vec_t<Backend> &b_max) const {
        auto bp = compute_breakpoints(backend, ctx, breakpoints, Σ, y, Ad, Ax, b_min, b_max);
        // Assume that compute_breakpoints already synchronized
        return ctx.call_broadcast([&] { return partition_breakpoints_default(bp); });
    }
} inline constexpr get_partitioned_breakpoints;

struct get_breakpoints_fn {
    template <class Backend>
    using vec_t = typename std::remove_cvref_t<Backend>::ineq_constr_vec_t;

    template <class Backend>
    // The get_breakpoints type should opt in to the tag to provide a custom
    // implementation.
        requires(guanaqo::tag_invocable<get_breakpoints_fn, Backend &, typename Backend::Context &,
                                        std::vector<Breakpoint> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &, const vec_t<Backend> &,
                                        const vec_t<Backend> &>)
    auto operator()(Backend &backend, typename Backend::Context &ctx,
                    std::vector<Breakpoint> &breakpoints, const vec_t<Backend> &Σ,
                    const vec_t<Backend> &y, const vec_t<Backend> &Ad, const vec_t<Backend> &Ax,
                    const vec_t<Backend> &b_min, const vec_t<Backend> &b_max) const
        noexcept(guanaqo::is_nothrow_tag_invocable_v<
                 get_breakpoints_fn, Backend &, typename Backend::Context &,
                 std::vector<Breakpoint> &, const vec_t<Backend> &, const vec_t<Backend> &,
                 const vec_t<Backend> &, const vec_t<Backend> &, const vec_t<Backend> &,
                 const vec_t<Backend> &>) {
        return guanaqo::guanaqo_tag_invoke(*this, backend, ctx, breakpoints, Σ, y, Ad, Ax, b_min,
                                           b_max);
    }

    template <class Backend>
    // Fallback implementation for unknown backends
        requires(!guanaqo::tag_invocable<get_breakpoints_fn, Backend &, typename Backend::Context &,
                                         std::vector<Breakpoint> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &, const vec_t<Backend> &,
                                         const vec_t<Backend> &>)
    BreakpointsResult operator()(Backend &backend, typename Backend::Context &ctx,
                                 std::vector<Breakpoint> &breakpoints, const vec_t<Backend> &Σ,
                                 const vec_t<Backend> &y, const vec_t<Backend> &Ad,
                                 const vec_t<Backend> &Ax, const vec_t<Backend> &b_min,
                                 const vec_t<Backend> &b_max) const {
        auto bp =
            get_partitioned_breakpoints(backend, ctx, breakpoints, Σ, y, Ad, Ax, b_min, b_max);
        return {.bp = bp, .ab_neg = partial_sum_negative(bp)};
    }
} inline constexpr get_breakpoints;

} // namespace CYQLONE_NAMESPACE::qpalm
