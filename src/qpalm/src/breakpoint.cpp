#include <cyqlone/qpalm/implementation/algorithms.hpp>
#include <cyqlone/qpalm/implementation/breakpoint.hpp>
#include <guanaqo/trace.hpp>
#include <numeric>

namespace cyqlone::qpalm {

PartitionedBreakpoints partition_breakpoints_default(std::span<Breakpoint> breakpoints) {
    GUANAQO_TRACE("linesearch partition", 0);
    using std::isfinite;
    // Move all infinite t[i] to the back
    const auto is_finite = [](Breakpoint b) { return isfinite(b.t); };
    const auto infinite  = partition(breakpoints, is_finite);
    const auto finite    = breakpoints.first(breakpoints.size() - infinite.size());
    // Move all nonpositive t[i] to the front
    const auto le_zero   = [](Breakpoint b) { return b.t <= 0; };
    const auto positive  = partition(finite, le_zero);
    const auto first_pos = finite.size() - positive.size();
    // Return the negative and positive partitions.
    return {
        .neg_bp = finite.first(first_pos),
        .pos_bp = finite.subspan(first_pos),
    };
}

ABSums partial_sum_negative(PartitionedBreakpoints breakpoints, real_t η, real_t β) {
    GUANAQO_TRACE("partial sum negative", 0);
    auto &neg_bp = breakpoints.neg_bp, &pos_bp = breakpoints.pos_bp;
    // Initialize a_j and b_j, where t_j is the first positive (see notes)
    auto a_plus  = std::transform_reduce(neg_bp.begin(), neg_bp.end(), ABSum_t{}, std::plus{},
                                         [](Breakpoint p) { return p.δ > 0 ? p.δ * p.δ : 0; });
    auto a_minus = std::transform_reduce(pos_bp.begin(), pos_bp.end(), ABSum_t{}, std::plus{},
                                         [](Breakpoint p) { return p.δ < 0 ? p.δ * p.δ : 0; });
    auto a       = a_plus + a_minus + η;
    auto b_plus  = std::transform_reduce(neg_bp.begin(), neg_bp.end(), ABSum_t{}, std::plus{},
                                         [](Breakpoint p) { return p.δ > 0 ? p.δ * p.α() : 0; });
    auto b_minus = std::transform_reduce(pos_bp.begin(), pos_bp.end(), ABSum_t{}, std::plus{},
                                         [](Breakpoint p) { return p.δ < 0 ? p.δ * p.α() : 0; });
    auto b       = b_plus + b_minus - β;
    return {.a = a, .b = b};
}

} // namespace cyqlone::qpalm
