#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/compress.hpp>
#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/hyhound.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/trsm.hpp>
#include <batmat/loop.hpp>

#include <numeric>

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_L(index_t l, index_t bi) {
    if (bi == 0) { // Last level
        const index_t j0 = 0, j1 = nJs.back(), nj = j1 - j0;
        auto W        = work_update.middle_cols(j0, nj);
        auto wΣ       = work_update_Σ.batch(0).middle_rows(j0, nj);
        bool update   = static_cast<double>(nj) < pcr_max_update_fraction * static_cast<double>(nx);
        bool update_y = static_cast<double>(nj) < cr_max_update_fraction * static_cast<double>(nx);
        bool do_update_pcr = solve_method == SolveMethod::PCR && update;
        if (do_update_pcr)
            update_pcr(W.batch(l & 3), W.batch((l + 2) & 3), wΣ);
        GUANAQO_TRACE("Update L", bi);
        // TODO: could be optimized further by recomputing if nj is larger than nx
        if (update_y || p >> 1 == 0)
            gemm_diag_add(W.batch(l & 3), W.batch((l + 2) & 3).transposed(), cr_Y.batch(0), wΣ);
        else
            gemm_neg(cr_Y.batch(p >> 1), cr_U.batch(p >> 1).transposed(), cr_Y.batch(0));
        if (solve_method == SolveMethod::PCR)
            syrk_diag_add(W.batch((l + 2) & 3), tril(cr_L.batch(0)), wΣ);
        if (!do_update_pcr)
            hyhound_diag(tril(pcr_L.batch(0)), W.batch((l + 2) & 3), wΣ);
        batmat::linalg::copy(wΣ, wΣ, with_rotate<-1>);
        batmat::linalg::copy(W.batch(l & 3), W.batch(l & 3), with_rotate<-1>);
        if (solve_method == SolveMethod::PCR)
            syrk_diag_add(W.batch(l & 3), tril(cr_L.batch(0)), wΣ);
        if (!do_update_pcr)
            hyhound_diag(tril(pcr_L.batch(0)), W.batch(l & 3), wΣ);
        // TODO: we should actually merge these two xshhud calls to
        //       make sure that the intermediate matrix does not become
        //       indefinite (although this shouldn't be an issue for
        //       QPALM)
        if (solve_method == SolveMethod::PCR && !update)
            factor_pcr();
    } else {
        GUANAQO_TRACE("Update L", bi);
        const index_t offset = 1 << l;
        const index_t j0 = bi == offset ? 0 : nJs[bi - 1 - offset], j1 = nJs[bi - 1 + offset],
                      nj = j1 - j0;
        auto UpL         = work_update.middle_cols(j0, nj).batch(l & 3);
        auto Σ           = work_update_Σ.batch(0).middle_rows(j0, nj);
        auto WQ          = work_hyh.batch(bi);
        // (L̃ | 0) = (L | Υ→ Υ← ) Q̆
        hyhound_diag(tril(cr_L.batch(bi)), UpL, Σ, WQ);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_U(index_t l, index_t bi) {
    GUANAQO_TRACE("Update U", bi);
    const index_t offset = 1 << l, i = bi >> (l + 1), j0 = bi == offset ? 0 : nJs[bi - 1 - offset],
                  j1 = nJs[bi - 1 + offset], nj = j1 - j0, jsplit = nJs[bi - 1] - j0;
    // alternating batches to make optimal use of the workspace
    static constexpr index_t w1b[]{2, 1, 2, 1};
    static constexpr index_t w2b[]{3, 1, 2, 1};
    auto W   = work_update.middle_cols(j0, nj);
    auto UpL = W.batch(l & 3);
    auto Σ   = work_update_Σ.batch(0).middle_rows(j0, nj);
    auto WQ  = work_hyh.batch(bi);
    hyhound_diag_apply(cr_U.batch(bi), W.batch((l + w1b[i & 3]) & 3), //
                       W.batch((l + w2b[i & 3]) & 3),                 //
                       UpL, Σ, WQ, 0, jsplit);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_Y(index_t l, index_t bi) {
    GUANAQO_TRACE("Update Y", bi);
    const index_t offset = 1 << l, i = bi >> (l + 1), j0 = bi == offset ? 0 : nJs[bi - 1 - offset],
                  j1 = nJs[bi - 1 + offset], nj = j1 - j0, jsplit = nJs[bi - 1] - j0;
    // alternating batches to make optimal use of the workspace
    static constexpr index_t w1b[]{1, 2, 1, 2};
    static constexpr index_t w2b[]{1, 2, 1, 3};
    auto W   = work_update.middle_cols(j0, nj);
    auto UpL = W.batch(l & 3);
    auto Σ   = work_update_Σ.batch(0).middle_rows(j0, nj);
    auto WQ  = work_hyh.batch(bi);
    hyhound_diag_apply(cr_Y.batch(bi), W.batch((l + w1b[i & 3]) & 3), //
                       W.batch((l + w2b[i & 3]) & 3),                 //
                       UpL, Σ, WQ, jsplit, -1);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void CyqloneSolver<VL, T, DefaultOrder>::update_pcr_level(index_t m, mut_batch_view<> WYU,
                                                          mut_batch_view<> WΣ) {
    constexpr index_t l    = Level;
    constexpr index_t rot0 = l == 0 ? 0 : 1 << (l - 1), rot1 = l == 0 ? 1 : 1 << (l - 1);
    const index_t ml = m << l;
    GUANAQO_TRACE("Update PCR", l);
    auto Σ = WΣ.top_rows(2 * ml);
    /*
     WL = [ Υ→[0]  | Υ←[0]  ]
     WY = [   0    | Υ→[+1] ]
     WU = [ Υ←[-1] |   0    ]
     */
    batmat::linalg::copy(Σ.top_rows(ml), Σ.bottom_rows(ml), with_rotate<+rot0>);
    batmat::linalg::copy(Σ.top_rows(ml), Σ.top_rows(ml), with_rotate<-rot1>);
    if constexpr (l < lvl) {
        auto WL = work_update_pcr_L.left_cols(2 * ml).batch(0);
        auto WU = WYU.right_cols(VL * m).left_cols(2 * ml);
        auto WY = WYU.left_cols(VL * m).right_cols(2 * ml);
        // Note that [ WY WU ] is contiguous (although this does not really help us since they have
        // different rotations)
        batmat::linalg::copy(WY.right_cols(ml), WL.left_cols(ml), with_rotate<-rot1>);
        batmat::linalg::copy(WU.left_cols(ml), WL.right_cols(ml), with_rotate<+rot0>);
        batmat::linalg::copy(WU, WU, with_rotate<-rot1>); // TODO: fuse with hyhound_diag_cyclic
        batmat::linalg::copy(WY, WY, with_rotate<+rot0>);
        hyhound_diag_cyclic(tril(pcr_L.batch(l)), WL, //
                            pcr_Y.batch(l), WY, WY,   //
                            pcr_U.batch(l), WU, WU, Σ, ml, 0);
    } else {
        batmat::linalg::copy(WYU, WYU, with_rotate<rot0>); // TODO: fuse with hyhound_diag
        hyhound_diag(tril(pcr_L.batch(l)), WYU, Σ);
    }
    // TODO: In the last level, we could maybe have WY and WU overlap (given proper masking
    //       in hyhound_diag_cyclic). The arrays WU and WY are suspiciously complementary ...
}

// TODO: write down the pseudocode for this algorithm in the appendix of the paper?
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_pcr(batch_view<> fwd, batch_view<> bwd,
                                                    batch_view<> Σfwd) {
#ifndef NDEBUG
    work_update_pcr_L.set_constant(std::numeric_limits<T>::quiet_NaN());
    work_update_pcr_Σ.set_constant(std::numeric_limits<T>::quiet_NaN());
    work_update_pcr_UY.set_constant(std::numeric_limits<T>::quiet_NaN());
#endif
    index_t m = fwd.cols();
    BATMAT_ASSUME(m == bwd.cols());
    auto WYU = work_update_pcr_UY.left_cols(2 * VL * m).batch(0);
    auto WY  = WYU.left_cols(VL * m); // WY and WU start in the middle of WYU and grow outwards
    auto WU  = WYU.right_cols(VL * m);
    auto Σ   = work_update_pcr_Σ.top_rows(2 * VL * m).batch(0);
    batmat::linalg::copy(bwd, WU.left_cols(m));
    batmat::linalg::copy(fwd, WY.right_cols(m));
    batmat::linalg::copy(Σfwd, Σ.top_rows(m));
    [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template update_pcr_level<Levels>(m, WYU, Σ), ...);
    }(std::make_integer_sequence<index_t, CyqloneSolver::lvl + 1>{});
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update(Context &ctx, view<> ΔΣ) {
    const index_t ti = ctx.index;
    // Call with Update=true, Solve=false for now (solve is separate phase)
    // TODO: merge solve step by passing ux and λ when implementing combined update+solve
    update_riccati_solve<true, false>(ctx, ΔΣ, {}, {});
    ctx.arrive_and_wait();
    if (ν2p(ti) == 0)
        update_L(0, ti);
    for (index_t l = 0; l < lP - lvl; ++l) {
        ctx.arrive_and_wait();
        const auto biU = add_wrap_ceil_p(ti, 1), biY = sub_wrap_ceil_p(ti, (1 << l) - 1);
        if (ν2p(biU) == l)
            update_U(l, biU);
        else if (ν2p(biY) == l)
            update_Y(l, biY);
        ctx.arrive_and_wait();
        if (ν2p(biY) == l + 1)
            update_L(l + 1, biY);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Update, bool Solve>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder>::update_riccati_solve(Context &ctx, view<> Σ,
                                                               [[maybe_unused]] mut_view<> ux,
                                                               [[maybe_unused]] mut_view<> λ) {
    const index_t c   = ctx.index;
    const index_t nyM = std::max(ny, ny_0 + ny_N);
    // TODO: special case nyM for c == 0
    // const index_t nyM = c == 0 ? std::max(ny, ny_0 + ny_N) : ny;
    const index_t dn  = c * n; // data batch index
    const index_t jn  = c * n; // stage index
    const index_t nux = nu + nx;
    auto R̂ŜQ̂          = riccati_LH.batch(c);
    auto B̂ = riccati_LAB.batch(c).right_cols(n * nu), Â = riccati_LAB.batch(c).left_cols(n * nx);
    auto Υ1 = riccati_Υ1.batch(c), Υ2 = riccati_Υ2.batch(c);
    auto wΣ = work_Σ.batch(c);

    index_t nJ = 0;
    if constexpr (Update) {
        GUANAQO_TRACE("Riccati update compress", jn);
        auto DC0 = Υ2.top_left(nu + nx, nyM);
        nJ       = compress_masks(data_Gᵀ.batch(dn), Σ.batch(dn), DC0, wΣ.top_rows(nyM));
        Υ2.bottom_left(nx, nJ).set_constant(0);
    }

    for (index_t i = 0; i < n; ++i) {
        index_t j = sub_wrap_N(jn, i);
        auto R̂ŜQ̂i = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto R̂Ŝi  = R̂ŜQ̂i.left_cols(nu);
        auto Q̂i   = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i   = B̂.middle_cols(i * nu, nu);
        auto Âi   = Â.middle_cols(i * nx, nx);

        index_t nJi = nJ;
        auto Υi     = (i & 1 ? Υ1 : Υ2).left_cols(nJi);
        if constexpr (Update) {
            if (nJi > 0) {
                GUANAQO_TRACE("Riccati update R", j);
                hyhound_diag_2(tril(R̂Ŝi), Υi.top_rows(nu + nx), B̂i, Υi.bottom_rows(nx),
                               wΣ.top_rows(nJi));
            }
        }
        if constexpr (Solve) {
            // Solve u ← LR̂⁻¹ u, x ← x - Ŝ u
            const index_t di = dn + i;
            auto ui = ux.batch(di).top_rows(nu), xi = ux.batch(di).bottom_rows(nx);
            auto R = R̂Ŝi.top_rows(nu), S = R̂Ŝi.bottom_rows(nx);
            trsm(tril(R), ui);
            gemv_sub(S, ui, xi);
            // λ(jₙ) += B̂ᵀ u
            auto λ_last = λ.batch(dn);
            gemv_add(B̂i, ui, λ_last);
        }
        if (i + 1 < n) {
            [[maybe_unused]] const auto k_next = sub_wrap_N(j, 1);
            const auto di_next                 = dn + i + 1;
            auto Υ_next                        = (i & 1 ? Υ2 : Υ1).left_cols(nJi + nyM);
            if constexpr (Update) {
                if (nJi > 0) {
                    GUANAQO_TRACE("Riccati update prop", k_next);
                    gemm(data_F.batch(di_next).transposed(), Υi.middle_rows(nu, nx),
                         Υ_next.top_left(nu + nx, nJi));
                    copy(Υi.bottom_rows(nx), Υ_next.bottom_left(nx, nJi));
                }
                {
                    GUANAQO_TRACE("Riccati update compress", k_next);
                    auto DC_next = Υ_next.block(0, nJi, nu + nx, nyM);
                    nJ += compress_masks(data_Gᵀ.batch(di_next), Σ.batch(di_next), DC_next,
                                         wΣ.middle_rows(nJi, nyM));
                    Υ_next.block(nu + nx, nJi, nx, nJ - nJi).set_constant(0);
                }
                if (nJi > 0) {
                    GUANAQO_TRACE("Riccati update Q", j);
                    gemm_diag_add(Υi.bottom_rows(nx), Υi.middle_rows(nu, nx).transposed(), Âi,
                                  wΣ.top_rows(nJi));
                    hyhound_diag(tril(Q̂i), Υi.middle_rows(nu, nx), wΣ.top_rows(nJi));
                }
            }
            if constexpr (Solve) {
                const index_t di = dn + i;
                auto xi = ux.batch(di).bottom_rows(nx), ux_next = ux.batch(di_next);
                auto λ_next = λ.batch(di_next), λ_last = λ.batch(dn);
                auto Q̂i = R̂ŜQ̂i.bottom_right(nx, nx);
                gemv_add(Âi, λ_next, λ_last); // λ(jₙ) += Âᵀ λ(j-1)
                auto w = work_Σ.batch(c).right_cols(1); // Reuse workspace
                trmm(tril(Q̂i).transposed(), λ_next, w); // w = LQᵀ(j) λ(j-1)
                trmm(tril(Q̂i), w);                      // w = LQ(j) LQᵀ(j) λ(j-1)
                compact_blas::xsub_copy(simdify(w), simdify(xi),
                                        simdify(w));       // w = x(j) - LQ(j) LQᵀ(j) λ(j-1)
                gemv_add(data_F.batch(di_next).transposed(), w, ux_next); // u(j-1) += BAᵀ(j-1) w
            }
        } else {
            const auto bi_upd = sub_wrap_ceil_p(c, 1);
            nJs[bi_upd]       = nJi;
            ctx.arrive_and_wait();
            if (ctx.is_master())
                std::inclusive_scan(begin(nJs), end(nJs), begin(nJs));
            ctx.arrive_and_wait(); // TODO: can be avoided by having each thread compute its own sums
            [[maybe_unused]] const index_t j0 = bi_upd == 0 ? 0 : nJs[bi_upd - 1], j1 = nJs[bi_upd];
            assert(nJi == j1 - j0);
            constexpr index_t wiA_table[]{0, 1, 0, 2};
            constexpr index_t wiI_table[]{2, 0, 1, 0};
            const index_t wiA = wiA_table[bi_upd & 3];
            const index_t wiI = wiI_table[bi_upd & 3];
            if constexpr (Update) {
                if (nJi > 0) {
                    GUANAQO_TRACE("Riccati update Q", j);
                    auto Q̂i_inv = R̂ŜQ̂i.block(nu - 1, nu, nx, nx);
                    hyhound_diag_riccati(tril(Q̂i), Υi.middle_rows(nu, nx), Âi, Υi.bottom_rows(nx),
                                         work_update.batch(wiA).middle_cols(j0, nJi), Q̂i_inv,
                                         work_update.batch(wiI).middle_cols(j0, nJi), wΣ.top_rows(nJi),
                                         c == 0); // TODO: optimize
                    compact_blas::xneg(simdify(work_update.batch(wiI).middle_cols(j0, nJi))); // TODO
                    c == 0 ? compact_blas::template xadd_neg_copy<-1>(
                                 simdify(work_update_Σ.batch(0).middle_rows(j0, nJi)),
                                 simdify(wΣ.top_rows(nJi)))
                           : compact_blas::template xadd_neg_copy<+0>(
                                 simdify(work_update_Σ.batch(0).middle_rows(j0, nJi)),
                                 simdify(wΣ.top_rows(nJi)));
                }
            }
            if constexpr (Solve) {
                const index_t di = dn + i;
                const index_t c_prev = sub_wrap_p(c, 1);
                const index_t dn_prev = c_prev * n;
                auto u1 = ux.batch(di).top_rows(nu), x1 = ux.batch(di).bottom_rows(nx);
                auto Q̂i = R̂ŜQ̂i.bottom_right(nx, nx);
                auto λ_last = λ.batch(dn);
                auto w = work_Σ.batch(c).right_cols(1); // Reuse workspace
                // w = LQ(j₁)⁻¹ λ(j₀)
                c == 0 ? trsm(tril(Q̂i), λ.batch(dn_prev), w, with_rotate_B<-1>)
                       : trsm(tril(Q̂i), λ.batch(dn_prev), w);
                // w = LQ(j₁)⁻¹ λ(j₀) - LAᵀ(j₁) λ(jₙ)
                gemv_sub(Âi.transposed(), λ_last, w);
                // w = LQ(j₁)⁻ᵀ(LQ(j₁)⁻¹ λ(j₀) - LAᵀ(j₁) λ(jₙ))
                trsm(tril(Q̂i).transposed(), w);
                // x(j₁) = LQ(j₁)⁻ᵀ(LQ(j₁)⁻¹ λ(j₀) - LAᵀ(j₁) λ(jₙ)) + q(j₁)
                compact_blas::xadd(simdify(x1), simdify(w));
                // u(j₁) = LR(j₁)⁻ᵀ(r(j₁) - LB(j₁)ᵀ λ(jₙ) - LS(j₁)ᵀ x(j₁))
                auto R = R̂Ŝi.top_rows(nu), S = R̂Ŝi.bottom_rows(nx);
                gemv_sub(B̂i.transposed(), λ_last, u1);
                gemv_sub(S.transposed(), x1, u1);
                trsm(tril(R).transposed(), u1);
            }
        }
    }
}

} // namespace CYQLONE_NS(cyqlone)::v2
