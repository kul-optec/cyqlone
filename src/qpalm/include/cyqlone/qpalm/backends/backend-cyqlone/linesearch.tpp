#pragma once

#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>

#include <algorithm>
#include <array>
#include <numeric>
#include <span>
#include <utility>
#include <vector>

namespace CYQLONE_NS(cyqlone::qpalm) {

template <index_t VL, StorageOrder DefaultOrder>
template <class T, size_t N>
void CyqloneBackend<VL, DefaultOrder>::merge_chunk(
    std::span<const T> chunk, size_t chunk_index, std::span<const std::array<size_t, N>> separators,
    std::span<T> out) {
    GUANAQO_TRACE("merge_chunk", 0, chunk.size());
    size_t num_chunks = separators.size();
    BATMAT_ASSUME(chunk_index < num_chunks);
    std::array<size_t, N> offsets{};
    for (size_t i = 0; i < N; ++i)
        for (size_t c = 0; c < chunk_index; ++c)
            offsets[i] += separators[c][i];
    for (size_t i = 0; i < N - 1; ++i)
        for (size_t c = chunk_index; c < num_chunks; ++c)
            offsets[i + 1] += separators[c][i];
    std::copy(chunk.begin(), chunk.begin() + separators[chunk_index][0], out.begin() + offsets[0]);
    for (size_t i = 1; i < N; ++i)
        std::copy(chunk.begin() + separators[chunk_index][i - 1],
                  chunk.begin() + separators[chunk_index][i], out.begin() + offsets[i]);
}

template <index_t VL, StorageOrder DefaultOrder>
BreakpointsResult CyqloneBackend<VL, DefaultOrder>::compute_partition_breakpoints(
    Context &ctx, std::vector<Breakpoint> &breakpoints, const ineq_constr_vec_t &Σ,
    const ineq_constr_vec_t &y, const ineq_constr_vec_t &Ad, const ineq_constr_vec_t &Ax,
    const ineq_constr_vec_t &b_min, const ineq_constr_vec_t &b_max) {
    auto t = get_timed(&Timings::breakpoints);
    using std::isfinite;
    using std::sqrt;
    // Allocate memory
    const index_t ny_M = std::max(ocp.ny, ocp.ny_0 + ocp.ny_N);
    ctx.run_single_sync([&] {
        const index_t m = ocp.ceil_N() * ny_M;
        breakpoints.resize(2 * m);
        breakpoints_temp.resize(2 * m);
        thread_indices.resize(ocp.p);
    });
    // Parallelization and vectorization
    auto thr_parts            = std::span{thread_indices}.subspan(0, ocp.p);
    const index_t ti          = ocp.riccati_thread_assignment(ctx);
    const index_t bpt_per_thr = 2 * ny_M * ocp.n * ocp.v;
    // Partition the breakpoints into a finite and an infinite part (per thread)
    Breakpoint *const fin_0 = breakpoints_temp.data() + ti * bpt_per_thr;
    Breakpoint *const inf_0 = fin_0 + bpt_per_thr;
    Breakpoint *fin = fin_0, *inf = inf_0;
    // Compute break points t[i] and intermediate values α[i] and δ[i]
    const auto brkpts_simd = [&](auto Σi, auto yi, auto Adi, auto Axi, auto li, auto ui) {
        const auto s  = sqrt(Σi);
        const auto δ2 = s * Adi, δ1 = -δ2;
        const auto α1 = (yi + Σi * (Axi - li)) / s, α2 = (Σi * (ui - Axi) - yi) / s;
        const auto t1 = α1 / δ1, t2 = α2 / δ2;
        BATMAT_FULLY_UNROLLED_FOR (int l = 0; l < ocp.v; ++l) {
            *(isfinite(t1[l]) ? fin++ : --inf) = {.t = t1[l], .δ = δ1[l]};
            *(isfinite(t2[l]) ? fin++ : --inf) = {.t = t2[l], .δ = δ2[l]};
        }
        // Invariant: finite values in [fin_0, fin) and infinite values in [inf, inf_0)
    };
    const auto brkpts_batch = [&]([[maybe_unused]] auto j, auto, auto Σj, auto yj, auto Adj,
                                  auto Axj, auto b_min_j, auto b_max_j) {
        GUANAQO_TRACE("linesearch breakpoints cyqlone", j);
        linalg::for_each_elementwise(brkpts_simd, Σj, yj, Adj, Axj, b_min_j, b_max_j);
    };
    ocp.foreach_stage(ctx, brkpts_batch, Σ, y, Ad, Ax, b_min, b_max);
    // Now partition the finite breakpoints into negative and positive parts.
    // Partitioning the chunk of each thread separately improves partitioning performance
    // later on in the line search because of branch prediction.
    auto [pos, large] = [&] {
        GUANAQO_TRACE("linesearch breakpoints cyqlone partition", di0);
        auto pos   = partition(fin_0, fin, [](Breakpoint p) { return p.t <= 0; }).begin();
        auto large = partition(pos, fin, [](Breakpoint p) { return p.t <= 1; }).begin();
        return std::pair{pos, large};
    }();
    // Store the separator indices for all threads, to merge the partitions in parallel later.
    thr_parts[ti][0]    = pos - fin_0;   // index of first positive breakpoint (per thread)
    thr_parts[ti][1]    = large - fin_0; // index of first breakpoint larger than 1 (per thread)
    thr_parts[ti][2]    = fin - fin_0;   // index of first infinite breakpoint (per thread)
    thr_parts[ti][3]    = inf_0 - fin_0; // total number of breakpoints (per thread)
    auto thr_parts_done = ctx.arrive();  // all-to-all
    // Compute the partial sums
    PartitionedBreakpoints pos_neg_bp{.neg_bp = std::span{fin_0, pos},
                                      .pos_bp = std::span{pos, fin}};
    auto ab_neg = partial_sum_negative(pos_neg_bp);
    // Synchronize the separator indices for all threads.
    ctx.wait(std::move(thr_parts_done));
    // Merge all local partitions of all threads into a single partitioned array.
    GUANAQO_TRACE("linesearch breakpoints cyqlone merge", di0);
    merge_chunk<Breakpoint, 4>(std::span{fin_0, inf_0}, ti, thr_parts, std::span{breakpoints});
    // Compute the total sums across all threads.
    auto ab_neg_and_merge_done = ctx.arrive_reduce(ab_neg, std::plus<>{});
    // Compute the final partition indices by summing the partition sizes of all threads.
    auto first_pos = std::accumulate(thr_parts.begin(), thr_parts.end(), breakpoints.begin(),
                                     [](auto it, auto &i) { return it += i[0]; }),
         first_inf = std::accumulate(thr_parts.begin(), thr_parts.end(), breakpoints.begin(),
                                     [](auto it, auto &i) { return it += i[2]; });
    // Wait for the total sums across all threads. Also synchronize the merged breakpoints.
    ab_neg = ctx.wait_reduce(std::move(ab_neg_and_merge_done));
    return {.bp     = {.neg_bp = std::span{breakpoints.begin(), first_pos},
                       .pos_bp = std::span{first_pos, first_inf}},
            .ab_neg = ab_neg};
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
