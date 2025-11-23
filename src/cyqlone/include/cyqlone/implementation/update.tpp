#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/compress.hpp>
#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/hyhound.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/loop.hpp>

#include <numeric>

namespace CYQLONE_NS(cyqlone) {

using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_level(index_t l, index_t biY) {
    GUANAQO_TRACE("update_level", biY);
    const index_t offset = 1 << l;
    const index_t i      = biY >> (l + 1);
    const index_t j0 = biY == offset ? 0 : nJs[biY - 1 - offset], j1 = nJs[biY - 1 + offset],
                  nj = j1 - j0, jsplit = nJs[biY - 1] - j0;
    constexpr index_t w3_out_lut[]{1, 0, 0, 1};
    const index_t w3_out = w3_out_lut[i & 3];
    auto W               = work_update.middle_cols(j0, nj);
    auto wΣ              = work_update_Σ.batch(0).middle_rows(j0, nj);
    BATMAT_ASSUME(biY != 0);
    if (i & 1) {
        hyhound_diag_cyclic(                                                            //
            tril(coupling_D.batch(biY)), W.batch(l & 3),                                //
            coupling_Y.batch(biY), W.batch((l + 2) % 4), W.batch((l + 2 + w3_out) % 4), //
            coupling_U.batch(biY), W.batch((l + 1) % 4), W.batch((l + 1) % 4),          //
            wΣ, jsplit, 0);
    } else {
        hyhound_diag_cyclic(                                                            //
            tril(coupling_D.batch(biY)), W.batch(l & 3),                                //
            coupling_Y.batch(biY), W.batch((l + 1) % 4), W.batch((l + 1) % 4),          //
            coupling_U.batch(biY), W.batch((l + 2) % 4), W.batch((l + 2 + w3_out) % 4), //
            wΣ, jsplit, 0);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void CyqloneSolver<VL, T, DefaultOrder>::update_pcr_level(index_t m, mut_batch_view<> WUY,
                                                          mut_batch_view<> WΣ) {
    constexpr index_t l      = Level;
    const index_t ml         = m << l;
    constexpr index_t shiftL = l == 0 ? 0 : 1 << (l - 1), shiftUY = l == 0 ? 1 : 1 << (l - 1);
    GUANAQO_TRACE("Update PCR", l);
    auto WL = work_update_pcr_L.left_cols(2 * ml).batch(0);
    auto WU = WUY.left_cols(2 * ml);
    auto WY = WUY.right_cols(2 * ml);
    auto Σ  = WΣ.top_rows(2 * ml);
    /*
     WL = [ Υ→[0]  | Υ←[0]  ]
     WY = [   0    | Υ→[+1] ]
     WU = [ Υ←[-1] |   0    ]
     */
    batmat::linalg::copy(Σ.top_rows(ml), Σ.bottom_rows(ml), with_rotate<+shiftL>);
    batmat::linalg::copy(Σ.top_rows(ml), Σ.top_rows(ml), with_rotate<-shiftUY>);
    // WL[:ml] = roll(WU[-ml:], +shiftL)
    batmat::linalg::copy(WY.right_cols(ml), WL.left_cols(ml), with_rotate<-shiftL>);
    // WL[ml:2*ml] = roll(WU[:ml], -shiftL)
    batmat::linalg::copy(WU.left_cols(ml), WL.right_cols(ml), with_rotate<+shiftL>);
    // WU = roll(WU, +shiftUY)
    batmat::linalg::copy(WU, WU, with_rotate<-shiftUY>); // TODO: fuse with hyhound_diag_cyclic
    // WY = roll(WY, -shiftUY)
    batmat::linalg::copy(WY, WY, with_rotate<+shiftUY>);
    hyhound_diag_cyclic(tril(pcr_L.batch(l)), WL, //
                        pcr_Y.batch(l), WY, WY,   //
                        pcr_U.batch(l), WU, WU, Σ, ml, 0);
    // TODO: In the last level, we could maybe have WY and WU overlap (given proper masking
    //       in hyhound_diag_cyclic). The arrays WU and WY are suspiciously complementary ...
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_pcr(batch_view<> fwd, batch_view<> bwd,
                                                    batch_view<> Σfwd) {
#ifndef NDEBUG
    work_update_pcr_L.set_constant(std::numeric_limits<T>::quiet_NaN());
    work_update_pcr_Σ.set_constant(std::numeric_limits<T>::quiet_NaN());
    work_update_pcr_UY.set_constant(std::numeric_limits<T>::quiet_NaN());
#endif
    index_t m = fwd.cols();
    BATMAT_ASSERT(m == bwd.cols());
    auto WUY = work_update_pcr_UY.left_cols(2 * VL * m).batch(0);
    auto Σ   = work_update_pcr_Σ.top_rows(2 * VL * m).batch(0);
    batmat::linalg::copy(bwd, WUY.left_cols(m));
    batmat::linalg::copy(fwd, WUY.right_cols(m), with_rotate<-1>);
    batmat::linalg::copy(Σfwd, Σ.top_rows(m));
    [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template update_pcr_level<Levels>(m, WUY, Σ), ...);
    }(std::make_integer_sequence<index_t, CyqloneSolver::lvl>{});
    constexpr index_t l      = lvl;
    const index_t ml         = m << l;
    constexpr index_t shiftL = l == 0 ? 0 : 1 << (l - 1), shiftUY = l == 0 ? 1 : 1 << (l - 1);
    GUANAQO_TRACE("Update PCR", l);
    batmat::linalg::copy(WUY, WUY, with_rotate<shiftL>); // TODO: fuse with hyhound_diag
    batmat::linalg::copy(Σ.top_rows(ml), Σ.bottom_rows(ml), with_rotate<+shiftL>);
    batmat::linalg::copy(Σ.top_rows(ml), Σ.top_rows(ml), with_rotate<-shiftUY>);
    hyhound_diag(tril(pcr_L.batch(lvl)), WUY, Σ);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update(Context &ctx, view<> ΔΣ) {
    if (ctx.index == 0)
        this->alt = true;
    const index_t ti = ctx.index;
    update_riccati(ctx, ΔΣ);
    for (index_t l = 0; l < lP - lvl; ++l) {
        ctx.arrive_and_wait();
        const index_t offset = 1 << l;
        const auto biY       = sub_wrap_PmV(ti, offset);
        if (is_active(l, biY))
            update_level(l, biY);
    }
    ctx.arrive_and_wait(); // TODO: remove and simply execute on the last thread
    const index_t l      = lP - lvl;
    const index_t offset = 1 << l;
    const auto biY       = sub_wrap_PmV(ti, offset);
    if (biY == 0) {
        const index_t j0 = 0, j1 = nJs.back(), nj = j1 - j0;
        auto W  = work_update.middle_cols(j0, nj);
        auto wΣ = work_update_Σ.batch(0).middle_rows(j0, nj);
        if (solve_method == SolveMethod::PCR && pcr_use_update)
            return update_pcr(W.batch(l & 3), W.batch((l + 2) & 3), wΣ);
        GUANAQO_TRACE("update_level last", biY);
        gemm_diag_add(W.batch(l & 3), W.batch((l + 2) & 3).transposed(), coupling_Y.batch(0), wΣ);
        if (solve_method == SolveMethod::PCR)
            syrk_diag_add(W.batch((l + 2) & 3), tril(coupling_D.batch(0)), wΣ);
        hyhound_diag(tril(pcr_L.batch(0)), W.batch((l + 2) & 3), wΣ);
        batmat::linalg::copy(wΣ, wΣ, with_rotate<-1>);
        batmat::linalg::copy(W.batch(l & 3), W.batch(l & 3), with_rotate<-1>);
        if (solve_method == SolveMethod::PCR)
            syrk_diag_add(W.batch(l & 3), tril(coupling_D.batch(0)), wΣ);
        hyhound_diag(tril(pcr_L.batch(0)), W.batch(l & 3), wΣ);
        // TODO: we should actually merge these two xshhud calls to
        //       make sure that the intermediate matrix does not become
        //       indefinite (although this shouldn't be an issue for
        //       QPALM)
        if (solve_method == SolveMethod::PCR)
            factor_pcr();
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_riccati(Context &ctx, view<> Σ) {
    const index_t ti         = ctx.index;
    const index_t nyM        = std::max(ny, ny_0 + ny_N);
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux        = nu + nx;
    auto R̂ŜQ̂                 = riccati_R̂ŜQ̂.batch(ti);
    auto B̂                   = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â                   = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    auto ΥΓ1                 = riccati_ΥΓ1.batch(ti);
    auto ΥΓ2                 = riccati_ΥΓ2.batch(ti);
    auto wΣ                  = work_Σ.batch(ti);

    index_t nJ;
    {
        GUANAQO_TRACE("Riccati update compress", k0);
        auto DC0 = ΥΓ2.top_left(nu + nx, nyM);
        nJ       = compress_masks(data_DCᵀ.batch(di0), Σ.batch(di0), DC0, wΣ.top_rows(nyM));
        ΥΓ2.bottom_left(nx, nJ).set_constant(0);
    }

    for (index_t i = 0; i < num_stages; ++i) {
        index_t k = sub_wrap_N(k0, i);
        auto R̂ŜQ̂i = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto R̂Ŝi  = R̂ŜQ̂i.left_cols(nu);
        auto Q̂i   = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i   = B̂.middle_cols(i * nu, nu);
        auto Âi   = Â.middle_cols(i * nx, nx);

        index_t nJi = nJ;
        auto ΥΓi    = ((i & 1) ? ΥΓ1 : ΥΓ2).left_cols(nJi);
        if (nJi > 0) {
            GUANAQO_TRACE("Riccati update R", k);
            hyhound_diag_2(tril(R̂Ŝi), ΥΓi.top_rows(nu + nx), B̂i, ΥΓi.bottom_rows(nx),
                           wΣ.top_rows(nJi));
        }
        if (i + 1 < num_stages) {
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            const auto di_next                 = di0 + i + 1;
            auto ΥΓ_next                       = ((i & 1) ? ΥΓ2 : ΥΓ1).left_cols(nJi + nyM);
            if (nJi > 0) {
                GUANAQO_TRACE("Riccati update prop", k_next);
                gemm(data_BA.batch(di_next).transposed(), ΥΓi.middle_rows(nu, nx),
                     ΥΓ_next.top_left(nu + nx, nJi));
                copy(ΥΓi.bottom_rows(nx), ΥΓ_next.bottom_left(nx, nJi));
            }
            {
                GUANAQO_TRACE("Riccati update compress", k_next);
                auto DC_next = ΥΓ_next.block(0, nJi, nu + nx, nyM);
                nJ += compress_masks(data_DCᵀ.batch(di_next), Σ.batch(di_next), DC_next,
                                     wΣ.middle_rows(nJi, nyM));
                ΥΓ_next.block(nu + nx, nJi, nx, nJ - nJi).set_constant(0);
            }
            if (nJi > 0) {
                GUANAQO_TRACE("Riccati update Q", k);
                gemm_diag_add(ΥΓi.bottom_rows(nx), ΥΓi.middle_rows(nu, nx).transposed(), Âi,
                              wΣ.top_rows(nJi));
                hyhound_diag(tril(Q̂i), ΥΓi.middle_rows(nu, nx), wΣ.top_rows(nJi));
            }
        } else {
            const auto bi_upd = sub_wrap_PmV(ti, 1);
            nJs[bi_upd]       = nJi;
            ctx.arrive_and_wait();
            if (ti == 0)
                std::inclusive_scan(begin(nJs), end(nJs), begin(nJs));
            ctx.arrive_and_wait(); // TODO: can be removed by having each thread compute its own sums
            [[maybe_unused]] const index_t j0 = bi_upd == 0 ? 0 : nJs[bi_upd - 1], j1 = nJs[bi_upd];
            assert(nJi == j1 - j0);
            constexpr index_t wiA_table[]{0, 1, 0, 2};
            constexpr index_t wiI_table[]{2, 0, 1, 0};
            const index_t wiA = wiA_table[bi_upd & 3];
            const index_t wiI = wiI_table[bi_upd & 3];
            if (nJi > 0) {
                GUANAQO_TRACE("Riccati update Q", k);
                auto Q̂i_inv = R̂ŜQ̂i.block(nu - 1, nu, nx, nx);
                hyhound_diag_riccati(tril(Q̂i), ΥΓi.middle_rows(nu, nx), Âi, ΥΓi.bottom_rows(nx),
                                     work_update.batch(wiA).middle_cols(j0, nJi), Q̂i_inv,
                                     work_update.batch(wiI).middle_cols(j0, nJi), wΣ.top_rows(nJi),
                                     ti == 0); // TODO: optimize
                compact_blas::xneg(simdify(work_update.batch(wiI).middle_cols(j0, nJi))); // TODO
                ti == 0 ? compact_blas::template xadd_neg_copy<-1>(
                              simdify(work_update_Σ.batch(0).middle_rows(j0, nJi)),
                              simdify(wΣ.top_rows(nJi)))
                        : compact_blas::xadd_neg_copy(
                              simdify(work_update_Σ.batch(0).middle_rows(j0, nJi)),
                              simdify(wΣ.top_rows(nJi)));
            }
        }
    }
}

} // namespace CYQLONE_NS(cyqlone)
