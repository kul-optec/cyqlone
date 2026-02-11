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
        thread_sums.resize(2 * ocp.p);
    });
    // Parallelization and vectorization
    auto as = std::span{thread_sums}.first(ocp.p), bs = std::span{thread_sums}.subspan(ocp.p);
    auto thr_parts = std::span{thread_indices}.subspan(0, ocp.p);
    // Compute break points t[i] and intermediate values α[i] and δ[i]
    std::span<Breakpoint> neg_bp, pos_bp;
    const index_t num_stages = ocp.n; // number of stages per thread
    const index_t ti         = ocp.riccati_thread_assignment(ctx);
    Breakpoint *const fin_0  = breakpoints_temp.data() + 2 * ti * num_stages * ny_M * ocp.v;
    Breakpoint *const inf_0  = fin_0 + 2 * num_stages * ny_M * ocp.v;
    Breakpoint *fin = fin_0, *inf = inf_0;
    const index_t di0 = ti * num_stages;
    for (index_t i = 0; i < num_stages; ++i) {
        const index_t di = di0 + i;
        GUANAQO_TRACE("linesearch breakpoints cyqlone", di);
        for (index_t r = 0; r < ny_M; ++r) {
            const auto Σi  = batmat::datapar::aligned_load<simd>(&Σ.batch(di)(0, r, 0)),
                       yi  = batmat::datapar::aligned_load<simd>(&y.batch(di)(0, r, 0)),
                       Adi = batmat::datapar::aligned_load<simd>(&Ad.batch(di)(0, r, 0)),
                       Axi = batmat::datapar::aligned_load<simd>(&Ax.batch(di)(0, r, 0)),
                       li  = batmat::datapar::aligned_load<simd>(&b_min.batch(di)(0, r, 0)),
                       ui  = batmat::datapar::aligned_load<simd>(&b_max.batch(di)(0, r, 0));
            const auto s   = sqrt(Σi);
            const auto δ2 = s * Adi, δ1 = -δ2;
            const auto α1 = (yi + Σi * (Axi - li)) / s, α2 = (Σi * (ui - Axi) - yi) / s;
            const auto t1 = α1 / δ1, t2 = α2 / δ2;
            BATMAT_FULLY_UNROLLED_FOR (int l = 0; l < ocp.v; ++l) {
                *(isfinite(t1[l]) ? fin++ : --inf) = {.t = t1[l], .δ = δ1[l]};
                *(isfinite(t2[l]) ? fin++ : --inf) = {.t = t2[l], .δ = δ2[l]};
            }
        }
    }
    // Partitioning the chunk of each thread separately improves partitioning performance
    // later on in the line search because of branch prediction.
    auto [pos, large] = [&] {
        GUANAQO_TRACE("linesearch breakpoints cyqlone partition", di0);
        auto pos   = partition(fin_0, fin, [](Breakpoint p) { return p.t <= 0; }).begin();
        auto large = partition(pos, fin, [](Breakpoint p) { return p.t <= 1; }).begin();
        return std::pair{pos, large};
    }();
    // Store the separator indices
    thr_parts[ti][0]    = pos - fin_0; // TODO: this is an all-to-all
    thr_parts[ti][1]    = large - fin_0;
    thr_parts[ti][2]    = fin - fin_0;
    thr_parts[ti][3]    = inf_0 - fin_0;
    auto thr_parts_done = ctx.arrive(); // TODO: use custom completion handler
    // Compute the partial sums
    PartitionedBreakpoints pos_neg_bp{.neg_bp = std::span{fin_0, pos},
                                      .pos_bp = std::span{pos, fin}};
    auto ab = partial_sum_negative(pos_neg_bp);
    as[ti]  = ab.a; // We don't use an atomic accumulator here for reproducibility (float
    bs[ti]  = ab.b; // addition is not associative, and thread order is nondeterministic)
    // Synchronize the separator indices for all threads
    ctx.wait(std::move(thr_parts_done));
    auto as_bs_done = ctx.arrive();
    // Merge all local partitions of all threads into a single partitioned array
    GUANAQO_TRACE("linesearch breakpoints cyqlone merge", di0);
    merge_chunk<Breakpoint, 4>(std::span{fin_0, inf_0}, ti, thr_parts, std::span{breakpoints});
    ctx.wait(std::move(as_bs_done));
    auto merge_done = ctx.arrive();
    // Compute the final partition indices
    auto first_pos = std::accumulate(thr_parts.begin(), thr_parts.end(), breakpoints.begin(),
                                     [](auto it, auto &i) { return it += i[0]; }),
         first_inf = std::accumulate(thr_parts.begin(), thr_parts.end(), breakpoints.begin(),
                                     [](auto it, auto &i) { return it += i[2]; });
    neg_bp         = std::span{breakpoints.begin(), first_pos};
    pos_bp         = std::span{first_pos, first_inf};
    // Compute the final sums
    auto a = std::accumulate(begin(as), end(as), ABSum_t{}),
         b = std::accumulate(begin(bs), end(bs), ABSum_t{});
    // Wait for the full partitioning
    ctx.wait(std::move(merge_done));
    return {.bp = {.neg_bp = neg_bp, .pos_bp = pos_bp}, .ab_neg = {.a = a, .b = b}};
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
