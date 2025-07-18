#pragma once

#include <cyqlone/config.hpp>

#include <guanaqo/trace.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <ranges>
#include <span>
#include <utility>
#include <vector>
#if KOQKATOO_QPALM_WITH_SKA_SORT // TODO
#include <ska_sort.hpp>
#endif

namespace cyqlone::qpalm {

struct Breakpoint {
    real_t t, δ, α;
};

template <class Vec>
struct LineSearch {
    using vec_t = Vec;

    std::pair<real_t, size_t> operator()(real_t η, real_t β, const vec_t &Σ, const vec_t &y,
                                         const vec_t &Ad, const vec_t &Ax, const vec_t &b_min,
                                         const vec_t &b_max);

    std::span<Breakpoint> compute_breakpoints(const vec_t &Σ, const vec_t &y, const vec_t &Ad,
                                              const vec_t &Ax, const vec_t &b_min,
                                              const vec_t &b_max);

    static std::array<std::span<Breakpoint>, 2>
    partition_breakpoints(std::span<Breakpoint> breakpoints);
    std::vector<Breakpoint> breakpoints;

    template <class R, class F>
    static void sort(R &&range, F key) {
        GUANAQO_TRACE("sort", 0, std::ranges::ssize(range));
#if KOQKATOO_QPALM_WITH_SKA_SORT
        ska_sort(std::ranges::begin(range), std::ranges::end(range), key);
#else
        std::sort(std::ranges::begin(range), std::ranges::end(range),
                  [&](auto a, auto b) { return key(a) < key(b); });
#endif
    }
};

/// Perform an exact line search on the augmented Lagrangian.
/// Implements Algorithm 2 in the QPALM paper.
///
/// @return τ Optimal step size @f$ \tau_\star @f$
template <class Vec>
std::pair<real_t, size_t>
LineSearch<Vec>::operator()(real_t η, ///< @f$ \eta = \inprod{d}{\xi} @f$
                            real_t β, ///< @f$ \beta = \inprod{d}{\grad\tilde f_k(x^{k,\nu})} @f$
                            const vec_t &Σ,     ///< Penalty factor @f$ \Sigma_k @f$ (diagonal)
                            const vec_t &y,     ///< Lagrange multipliers @f$ y^k @f$
                            const vec_t &Ad,    ///< Matrix-vector product @f$ A d @f$
                            const vec_t &Ax,    ///< Matrix-vector product @f$ A x^{k,\nu} @f$
                            const vec_t &b_min, ///< Constraint lower bound @f$ b_\mathrm{min} @f$
                            const vec_t &b_max  ///< Constraint upper bound @f$ b_\mathrm{max} @f$
) {
    using std::abs;
    // Compute breakpoints t[i] and intermediate values α[i] and δ[i]
    auto breakpoints = compute_breakpoints(Σ, y, Ad, Ax, b_min, b_max);
    // Isolate non-finite entries, and sort t[i] in ascending order. Then split
    // the arrays into a nonpositive and a positive part for t.
    const auto [neg_bp, pos_bp] = partition_breakpoints(breakpoints);

    GUANAQO_TRACE("linesearch find", 0);
    // Initialize a_j and b_j, where t_j is the first positive (see notes)
    auto a_plus_  = [](Breakpoint nb) { return nb.δ > 0 ? nb.δ * nb.δ : 0; };
    auto a_minus_ = [](Breakpoint pb) { return pb.δ < 0 ? pb.δ * pb.δ : 0; };
    real_t a_plus =
        std::transform_reduce(neg_bp.begin(), neg_bp.end(), real_t{}, std::plus{}, a_plus_);
    real_t a_minus =
        std::transform_reduce(pos_bp.begin(), pos_bp.end(), real_t{}, std::plus{}, a_minus_);
    real_t a      = η + a_plus + a_minus;
    auto b_plus_  = [](Breakpoint nb) { return nb.δ > 0 ? nb.δ * nb.α : 0; };
    auto b_minus_ = [](Breakpoint pb) { return pb.δ < 0 ? pb.δ * pb.α : 0; };
    real_t b_plus =
        std::transform_reduce(neg_bp.begin(), neg_bp.end(), real_t{}, std::plus{}, b_plus_);
    real_t b_minus =
        std::transform_reduce(pos_bp.begin(), pos_bp.end(), real_t{}, std::plus{}, b_minus_);
    real_t b = β - b_plus - b_minus;

    // Handle the common case first: if the smallest t already has ψʹ ≥ 0, then
    // there's no need to sort all breakpoints.
    if (pos_bp.size() > 0) {
        if (real_t ψʹ = pos_bp[0].t * a + b; ψʹ >= 0)
            return {-b / a, 0};
        // Otherwise, we should sort the breakpoints (although in theory we
        // don't have to sort all of them, in practice this is easier).
        sort(pos_bp, [](Breakpoint b) { return b.t; });
    }
    // Find the first i for which ψʹ(t[i]) ≥ 0
    for (size_t i = 0; i < pos_bp.size(); ++i) {
        if (real_t ψʹ = pos_bp[i].t * a + b; ψʹ >= 0)
            return {i == 0 ? 1 : -b / a, i}; // linear interpolation
        // Recursive update formula for a_j and b_j (see notes)
        a += pos_bp[i].δ * abs(pos_bp[i].δ);
        b -= pos_bp[i].α * abs(pos_bp[i].δ);
    }
    // No positive entries, or solution lies above all breakpoints
    return {-b / a, pos_bp.size()}; // extrapolate
}

/// Compute the break points t[i] using formula (3.6) in the QPALM paper.
/// @return t, α, δ
template <class Vec>
auto LineSearch<Vec>::compute_breakpoints(const vec_t &Σ, const vec_t &y, const vec_t &Ad,
                                          const vec_t &Ax, const vec_t &b_min, const vec_t &b_max)
    -> std::span<Breakpoint> {
    GUANAQO_TRACE("linesearch breakpoints", 0);
    using std::sqrt;
    // Allocate memory
    auto m = static_cast<size_t>(y.size());
    breakpoints.resize(2 * m);
    // Compute break points t[i] and intermediate values α[i] and δ[i]
    auto indices = std::views::iota(index_t{});
    for (auto [Σi, yi, Adi, Axi, li, ui, i] :
         std::views::zip(Σ, y, Ad, Ax, b_min, b_max, indices)) {
        breakpoints[m + i].δ = sqrt(Σi) * Adi;
        breakpoints[i].δ     = -breakpoints[m + i].δ;
        breakpoints[i].α     = (yi + Σi * (Axi - li)) / sqrt(Σi);
        breakpoints[m + i].α = (Σi * (ui - Axi) - yi) / sqrt(Σi);
        breakpoints[i].t     = breakpoints[i].α / breakpoints[i].δ;
        breakpoints[m + i].t = breakpoints[m + i].α / breakpoints[m + i].δ;
    }
    return std::span{breakpoints};
}

/// Moves any non-finite elements in t to the end of the range, and all negative
/// elements to the front. Finally, it moves the smallest positive element to
/// the beginning of the positive partition. Returns the negative and positive
/// partitions.
template <class Vec>
auto LineSearch<Vec>::partition_breakpoints(std::span<Breakpoint> breakpoints)
    -> std::array<std::span<Breakpoint>, 2> {
    GUANAQO_TRACE("linesearch partition", 0);
    using std::isfinite;
    // Move all infinite t[i] to the back
    const auto get_t     = [](Breakpoint b) { return b.t; };
    const auto is_finite = [](real_t t) { return isfinite(t); };
    const auto infinite  = std::ranges::partition(breakpoints, is_finite, get_t);
    const auto finite    = breakpoints.first(breakpoints.size() - infinite.size());
    // Move all nonpositive t[i] to the front
    const auto le_zero   = [](real_t t) { return t <= 0; };
    const auto positive  = std::ranges::partition(finite, le_zero, get_t);
    const auto first_pos = finite.size() - positive.size();
    // Find the smallest positive t[i] and move it to the beginning of positive
    if (positive.size() > 0) {
        const auto smallest     = std::ranges::min_element(positive, {}, get_t);
        const auto first_pos_it = std::ranges::begin(positive);
        if (first_pos_it != smallest)
            std::ranges::iter_swap(first_pos_it, smallest);
    }
    // Return the negative and positive partitions.
    return {
        finite.first(first_pos),
        finite.subspan(first_pos),
    };
}

} // namespace cyqlone::qpalm
