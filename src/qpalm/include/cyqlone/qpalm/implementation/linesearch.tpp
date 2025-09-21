#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/neumaier.hpp>
#include <cyqlone/qpalm/implementation/algorithms.hpp>
#include <cyqlone/qpalm/implementation/breakpoint.hpp>
#include <cyqlone/qpalm/implementation/breakpoint.tpp>

#include <batmat/assume.hpp>
#include <guanaqo/trace.hpp>
#include <cmath>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#ifndef NDEBUG
#define LINE_SEARCH_COMPARE_IMPLEMENTATIONS 1
#else
#define LINE_SEARCH_COMPARE_IMPLEMENTATIONS 0
#endif

namespace cyqlone::qpalm {

#if CYQLONE_COMPENSATE_SUM_LINE_SEARCH
using NSum = NeumaierSum<real_t>;
#else
using NSum = real_t;
#endif

struct LineSearchSettings {
    bool find_smallest_breakpoint_first = false;
};

template <class Vec>
struct LineSearch {
    using vec_t = Vec;
    LineSearchSettings settings;
    std::vector<Breakpoint> breakpoints;

    std::pair<real_t, size_t> operator()(auto &backend, real_t η, real_t β, const vec_t &Σ,
                                         const vec_t &y, const vec_t &Ad, const vec_t &Ax,
                                         const vec_t &b_min, const vec_t &b_max);

    static std::pair<real_t, size_t> find_stepsize_base(NSum a, NSum b, size_t i0,
                                                        std::span<Breakpoint> pos_bp);
    static std::pair<real_t, size_t>
    find_stepsize(NSum a, NSum b, size_t i0, std::span<Breakpoint> pos_bp, bool partition_1 = true);
};

template <class Vec>
std::pair<real_t, size_t> LineSearch<Vec>::find_stepsize_base(NSum a, NSum b, size_t i0,
                                                              std::span<Breakpoint> pos_bp) {
    using std::abs;
    // Order all breakpoints by increasing ti
    sort(pos_bp, [](Breakpoint b) { return b.t; });
    // Find the first i for which ψʹ(t[i]) ≥ 0
    for (size_t i = 0; i < pos_bp.size(); ++i) {
        if (real_t ψʹ = pos_bp[i].t * a - b; ψʹ >= 0)
            return {b / a, i0 + i}; // linear interpolation
        // Recursive update formula for a_j and b_j (see notes)
        a += pos_bp[i].δ * abs(pos_bp[i].δ);
        b += pos_bp[i].α() * abs(pos_bp[i].δ);
    }
    // No positive entries, or solution lies above all breakpoints
    return {b / a, i0 + pos_bp.size()}; // extrapolate
}

template <class Vec>
std::pair<real_t, size_t> LineSearch<Vec>::find_stepsize(NSum a, NSum b, size_t i0,
                                                         std::span<Breakpoint> pos_bp,
                                                         bool partition_1) {
    using std::abs;
    if (pos_bp.size() < 8)
        return find_stepsize_base(a, b, i0, pos_bp);
    const auto [i_mid, mid] = [&] {
        if (partition_1) {
            auto mid = std::ranges::begin(partition(pos_bp, [](Breakpoint b) { return b.t <= 1; }));
            auto i_mid = static_cast<std::size_t>(mid - std::ranges::begin(pos_bp));
            return std::make_pair(i_mid, mid);
        } else {
            auto i_mid = pos_bp.size() / 4;
            auto mid   = std::ranges::next(pos_bp.begin(), static_cast<std::ptrdiff_t>(i_mid));
            nth_element(pos_bp, mid, [](Breakpoint b) { return b.t; });
            return std::make_pair(i_mid, mid);
        }
    }();
    auto left = pos_bp.first(i_mid + 1), right = pos_bp.subspan(i_mid); // Both halves contain mid

    // Recursive update formula for a_j and b_j (see notes)
    NSum a_mid = a, b_mid = b;
    for (auto bp : left.first(i_mid)) {
        a_mid += bp.δ * abs(bp.δ);
        b_mid += bp.α() * abs(bp.δ);
    }
    // Check dir deriv at mid
    const real_t ψʹ_mid = mid->t * a_mid - b_mid;
    if (ψʹ_mid >= 0) { // zero crossing lies in the left half
        return find_stepsize(a, b, i0, left, false);
    } else { // zero crossing lies in the right half
        return find_stepsize(a_mid, b_mid, i0 + i_mid, right, false);
    }
}

/// Perform an exact line search on the augmented Lagrangian.
/// Implements Algorithm 2 in the QPALM paper.
///
/// @return τ Optimal step size @f$ \tau_\star @f$
template <class Vec>
std::pair<real_t, size_t>
LineSearch<Vec>::operator()(auto &backend, real_t η, ///< @f$ \eta = \inprod{d}{\xi} @f$
                            real_t β, ///< @f$ \beta = \inprod{d}{\grad\tilde f_k(x^{k,\nu})} @f$
                            const vec_t &Σ,     ///< Penalty factor @f$ \Sigma_k @f$ (diagonal)
                            const vec_t &y,     ///< Lagrange multipliers @f$ y^k @f$
                            const vec_t &Ad,    ///< Matrix-vector product @f$ A d @f$
                            const vec_t &Ax,    ///< Matrix-vector product @f$ A x^{k,\nu} @f$
                            const vec_t &b_min, ///< Constraint lower bound @f$ b_\mathrm{min} @f$
                            const vec_t &b_max  ///< Constraint upper bound @f$ b_\mathrm{max} @f$
) {
    using std::abs;
    // Compute breakpoints t[i] and intermediate values α[i] and δ[i], then partition them by t[i]
    // and compute a0 and b0, summing over all negative breakpoints.
    BreakpointsResult bp = get_breakpoints(backend, breakpoints, Σ, y, Ad, Ax, b_min, b_max);
    auto pos_bp          = bp.bp.pos_bp;
    auto [a, b]          = bp.ab_neg;
    a += η;
    b -= β;
    index_t i = 0;

#if LINE_SEARCH_COMPARE_IMPLEMENTATIONS
    std::vector<Breakpoint> pos_bp_debug(pos_bp.begin(), pos_bp.end());
    auto step_size_debug = find_stepsize_base(a, b, 0, std::span{pos_bp_debug});
#endif

    // Handle the trivial cases first:
    // If there are no positive breakpoints, then ψ is simply quadratic on [0, +∞), so we can safely
    // accept unit step size.
    if (pos_bp.size() == 0)
        return {1, 0};
    // Optimization: check the first interval for an early return if there is no active set change.
    // If the smallest breakpoint already has ψʹ ≥ 0, then there's no need to sort all breakpoints.
    // Find the smallest positive t[i] and move it to the beginning of positive
    if (settings.find_smallest_breakpoint_first) {
        const auto smallest     = min_element(pos_bp, [](Breakpoint b) { return b.t; });
        const auto first_pos_it = std::ranges::begin(pos_bp);
        if (first_pos_it != smallest)
            std::ranges::iter_swap(first_pos_it, smallest);
        if (real_t ψʹ0 = pos_bp[0].t * a - b; ψʹ0 >= 0)
            return {1, 0};
        // Otherwise, skip the first breakpoint, and perform an actual search.
        a += pos_bp[0].δ * abs(pos_bp[0].δ);
        b += pos_bp[0].α() * abs(pos_bp[0].δ);
        ++i;
        pos_bp = pos_bp.subspan(1);
    }

    GUANAQO_TRACE("linesearch find stepsize", 0);
    auto step_size = find_stepsize(a, b, i, pos_bp);
#if LINE_SEARCH_COMPARE_IMPLEMENTATIONS
    BATMAT_ASSERT(abs(step_size.first - step_size_debug.first) <
                  real_t(1e4) * std::numeric_limits<real_t>::epsilon());
    BATMAT_ASSERT(step_size.second == step_size_debug.second);
#endif
    return step_size;
}

} // namespace cyqlone::qpalm
