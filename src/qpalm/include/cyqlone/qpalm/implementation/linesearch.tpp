#pragma once

#include <cyqlone/config.hpp>

#include <batmat/assume.hpp>
#include <guanaqo/trace.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <ranges>
#include <span>
#include <utility>
#include <vector>
#if CYQLONE_WITH_SKA_SORT
#include <ska_sort.hpp>
#endif

#ifndef NDEBUG
#define LINE_SEARCH_COMPARE_IMPLEMENTATIONS 1
#else
#define LINE_SEARCH_COMPARE_IMPLEMENTATIONS 0
#endif

namespace cyqlone::qpalm {

struct Breakpoint {
    // t = α/δ   <=>   α = t δ
    real_t t, δ;
    [[gnu::always_inline, nodiscard]] real_t α() const { return t * δ; }
};

template <class T>
class NeumaierSum {
    static_assert(std::is_floating_point_v<T>);

    T sum, compensation = 0;
    NeumaierSum(T sum, T compensation) : sum(sum), compensation(compensation) {}

  public:
    NeumaierSum(T value = {}) : sum(value), compensation(0) {}
    operator T() const { return sum + compensation; }

    NeumaierSum operator-() const { return {-sum, -compensation}; }
    NeumaierSum &operator+=(T v) {
        using std::abs;
        T t = sum + v;
        if (abs(sum) >= abs(v))
            compensation += (sum - t) + v;
        else
            compensation = (v - t) + sum;
        sum = t;
        return *this;
    }
    NeumaierSum &operator+=(const NeumaierSum &other) {
        *this += other.sum;
        *this += other.compensation;
        return *this;
    }
    NeumaierSum &operator-=(T v) { return *this += -v; }
    NeumaierSum &operator-=(const NeumaierSum &other) {
        *this -= other.sum;
        *this -= other.compensation;
        return *this;
    }

    friend NeumaierSum operator+(NeumaierSum lhs, T rhs) {
        lhs += rhs;
        return lhs;
    }
    friend NeumaierSum operator+(T lhs, NeumaierSum rhs) {
        rhs += lhs;
        return rhs;
    }
    friend NeumaierSum operator+(NeumaierSum lhs, NeumaierSum rhs) {
        lhs += rhs;
        return lhs;
    }

    friend NeumaierSum operator-(NeumaierSum lhs, T rhs) {
        lhs -= rhs;
        return lhs;
    }
    friend NeumaierSum operator-(T lhs, NeumaierSum rhs) { return lhs + (-rhs); }
    friend NeumaierSum operator-(NeumaierSum lhs, const NeumaierSum &rhs) {
        lhs -= rhs;
        return lhs;
    }
};

#if CYQLONE_COMPENSATE_SUM_LINE_SEARCH
using NSum = NeumaierSum<real_t>;
#else
using NSum = real_t;
#endif

template <class Vec>
struct LineSearch {
    using vec_t = Vec;

    std::pair<real_t, size_t> operator()(auto &backend, real_t η, real_t β, const vec_t &Σ,
                                         const vec_t &y, const vec_t &Ad, const vec_t &Ax,
                                         const vec_t &b_min, const vec_t &b_max);

    std::span<Breakpoint> compute_breakpoints(const vec_t &Σ, const vec_t &y, const vec_t &Ad,
                                              const vec_t &Ax, const vec_t &b_min,
                                              const vec_t &b_max);

    static std::array<std::span<Breakpoint>, 2>
    partition_breakpoints(std::span<Breakpoint> breakpoints);
    std::vector<Breakpoint> breakpoints;

    static std::pair<NSum, NSum> partial_sum_negative(real_t η, real_t β,
                                                      std::span<const Breakpoint> pos_bp,
                                                      std::span<const Breakpoint> neg_bp);
    static std::pair<real_t, size_t> find_stepsize_base(NSum a, NSum b, size_t i0,
                                                        std::span<Breakpoint> pos_bp);
    static std::pair<real_t, size_t> find_stepsize(NSum a, NSum b, size_t i0,
                                                   std::span<Breakpoint> pos_bp);

    template <class R, class F>
    static void sort(R &&range, F key) {
        GUANAQO_TRACE("sort", 0, std::ranges::ssize(range));
#if CYQLONE_WITH_SKA_SORT
        ska_sort(std::ranges::begin(range), std::ranges::end(range), key);
#else
        std::sort(std::ranges::begin(range), std::ranges::end(range),
                  [&](auto a, auto b) { return key(a) < key(b); });
#endif
    }

    template <class R, class I, class F>
    static void nth_element(R &&range, I mid, F key) {
        GUANAQO_TRACE("nth_element", 0, std::ranges::ssize(range));
        std::nth_element(std::ranges::begin(range), mid, std::ranges::end(range),
                         [&](auto a, auto b) { return key(a) < key(b); });
    }
};

template <class Vec>
std::pair<NSum, NSum> LineSearch<Vec>::partial_sum_negative(real_t η, real_t β,
                                                            std::span<const Breakpoint> pos_bp,
                                                            std::span<const Breakpoint> neg_bp) {
    GUANAQO_TRACE("partial sum negative", 0);
    // Initialize a_j and b_j, where t_j is the first positive (see notes)
    auto a_plus_ = [](Breakpoint nb) { return nb.δ > 0 ? nb.δ * nb.δ : 0; };
    auto a_mins_ = [](Breakpoint pb) { return pb.δ < 0 ? pb.δ * pb.δ : 0; };
    NSum a_plus = std::transform_reduce(neg_bp.begin(), neg_bp.end(), NSum{}, std::plus{}, a_plus_);
    NSum a_mins = std::transform_reduce(pos_bp.begin(), pos_bp.end(), NSum{}, std::plus{}, a_mins_);
    NSum a      = η + a_plus + a_mins;
    auto b_plus_ = [](Breakpoint nb) { return nb.δ > 0 ? nb.δ * nb.α() : 0; };
    auto b_mins_ = [](Breakpoint pb) { return pb.δ < 0 ? pb.δ * pb.α() : 0; };
    NSum b_plus = std::transform_reduce(neg_bp.begin(), neg_bp.end(), NSum{}, std::plus{}, b_plus_);
    NSum b_mins = std::transform_reduce(pos_bp.begin(), pos_bp.end(), NSum{}, std::plus{}, b_mins_);
    NSum b      = β - b_plus - b_mins;
    return {a, b};
}

template <class Vec>
std::pair<real_t, size_t> LineSearch<Vec>::find_stepsize_base(NSum a, NSum b, size_t i0,
                                                              std::span<Breakpoint> pos_bp) {
    using std::abs;
    // Order all breakpoints by increasing ti
    sort(pos_bp, [](Breakpoint b) { return b.t; });
    // Find the first i for which ψʹ(t[i]) ≥ 0
    for (size_t i = 0; i < pos_bp.size(); ++i) {
        if (real_t ψʹ = pos_bp[i].t * a + b; ψʹ >= 0)
            return {-b / a, i0 + i}; // linear interpolation
        // Recursive update formula for a_j and b_j (see notes)
        a += pos_bp[i].δ * abs(pos_bp[i].δ);
        b -= pos_bp[i].α() * abs(pos_bp[i].δ);
    }
    // No positive entries, or solution lies above all breakpoints
    return {-b / a, i0 + pos_bp.size()}; // extrapolate
}

template <class Vec>
std::pair<real_t, size_t> LineSearch<Vec>::find_stepsize(NSum a, NSum b, size_t i0,
                                                         std::span<Breakpoint> pos_bp) {
    using std::abs;
    BATMAT_ASSERT(!pos_bp.empty());
    if (pos_bp.size() < 8)
        return find_stepsize_base(a, b, i0, pos_bp);
    const auto i_mid = pos_bp.size() / 8;
    BATMAT_ASSERT(i_mid < pos_bp.size());
    const auto mid = std::ranges::next(pos_bp.begin(), static_cast<std::ptrdiff_t>(i_mid));
    nth_element(pos_bp, mid, [](Breakpoint b) { return b.t; });
    auto left = pos_bp.first(i_mid + 1), right = pos_bp.subspan(i_mid); // Both halves contain mid

    // Recursive update formula for a_j and b_j (see notes)
    NSum a_mid = a, b_mid = b;
    for (auto bp : left.first(i_mid)) {
        a_mid += bp.δ * abs(bp.δ);
        b_mid -= bp.α() * abs(bp.δ);
    }
    // Check dir deriv at mid
    const real_t ψʹ_mid = mid->t * a_mid + b_mid;
    if (ψʹ_mid >= 0) { // zero crossing lies in the left half
        return find_stepsize(a, b, i0, left);
    } else { // zero crossing lies in the right half
        return find_stepsize(a_mid, b_mid, i0 + i_mid, right);
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
    // Compute breakpoints t[i] and intermediate values α[i] and δ[i]
    auto breakpoints = [&] {
        if constexpr (requires {
                          backend.compute_breakpoints(this->breakpoints, Σ, y, Ad, Ax, b_min,
                                                      b_max);
                      })
            return backend.compute_breakpoints(this->breakpoints, Σ, y, Ad, Ax, b_min, b_max);
        else
            return compute_breakpoints(Σ, y, Ad, Ax, b_min, b_max);
    }();
    // Isolate non-finite entries, and sort t[i] in ascending order. Then split
    // the arrays into a nonpositive and a positive part for t.
    const auto [neg_bp, pos_bp] = partition_breakpoints(breakpoints);
    // Compute a0 and b0, summing over all negative breakpoints.
    auto [a, b] = partial_sum_negative(η, β, pos_bp, neg_bp);

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
    if (real_t ψʹ0 = pos_bp[0].t * a + b; ψʹ0 >= 0)
        return {1, 0};
    // Otherwise, skip the first breakpoint, and perform an actual search.
    a += pos_bp[0].δ * abs(pos_bp[0].δ);
    b -= pos_bp[0].α() * abs(pos_bp[0].δ);

    GUANAQO_TRACE("find stepsize", 0);
    auto step_size = find_stepsize(a, b, 1, pos_bp.subspan(1));
#if LINE_SEARCH_COMPARE_IMPLEMENTATIONS
    BATMAT_ASSERT(abs(step_size.first - step_size_debug.first) <
                  real_t(1e4) * std::numeric_limits<real_t>::epsilon());
    BATMAT_ASSERT(step_size.second == step_size_debug.second);
#endif
    return step_size;
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
        const auto s         = sqrt(Σi);
        breakpoints[m + i].δ = s * Adi;
        breakpoints[i].δ     = -breakpoints[m + i].δ;
        breakpoints[i].t     = (yi + Σi * (Axi - li)) / (s * breakpoints[i].δ);
        breakpoints[m + i].t = (Σi * (ui - Axi) - yi) / (s * breakpoints[m + i].δ);
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
    const auto is_finite = [](Breakpoint b) { return isfinite(b.t); };
    const auto infinite  = std::ranges::partition(breakpoints, is_finite);
    const auto finite    = breakpoints.first(breakpoints.size() - infinite.size());
    // Move all nonpositive t[i] to the front
    const auto le_zero   = [](Breakpoint b) { return b.t <= 0; };
    const auto positive  = std::ranges::partition(finite, le_zero);
    const auto first_pos = finite.size() - positive.size();
    // Find the smallest positive t[i] and move it to the beginning of positive
    if (positive.size() > 0) {
        const auto get_t        = [](Breakpoint b) { return b.t; };
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
