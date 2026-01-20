#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/compress.hpp>
#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/hyhound.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/loop.hpp>

#include <numeric>

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_L(index_t l, index_t iL) {
    if (iL == 0) { // Last level
        const index_t j0 = 0, j1 = m_update.back(), nj = j1 - j0;
        auto W        = work_update.middle_cols(j0, nj);
        auto wΣ       = work_update_Σ.batch(0).middle_rows(j0, nj);
        bool update   = static_cast<double>(nj) < pcr_max_update_fraction * static_cast<double>(nx);
        bool update_y = static_cast<double>(nj) < cr_max_update_fraction * static_cast<double>(nx);
        bool do_update_pcr = solve_method == SolveMethod::PCR && update;
        if (do_update_pcr)
            update_pcr(W.batch(l & 3), W.batch((l + 2) & 3), wΣ);
        GUANAQO_TRACE("Update L", iL);
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
        GUANAQO_TRACE("Update L", iL);
        const index_t offset = 1 << l;
        const index_t j0     = iL == offset ? 0 : m_update[iL - 1 - offset],
                      j1 = m_update[std::min(iL - 1 + offset, p - 1)], nj = j1 - j0;
        auto UpL = work_update.middle_cols(j0, nj).batch(l & 3);
        auto Σ   = work_update_Σ.batch(0).middle_rows(j0, nj);
        auto WQ  = work_hyh.batch(iL);
        // (L̃ | 0) = (L | Υ→ Υ← ) Q̆
        hyhound_diag(tril(cr_L.batch(iL)), UpL, Σ, WQ);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_U(index_t l, index_t iU) {
    if constexpr (VL == 1)
        if (iU >= p) // happens in cases where p is not a power of two
            return;
    GUANAQO_TRACE("Update U", iU);
    const index_t offset = 1 << l, i = iU >> (l + 1),
                  j0 = iU == offset ? 0 : m_update[iU - 1 - offset],
                  j1 = m_update[std::min(iU - 1 + offset, p - 1)], nj = j1 - j0,
                  jsplit = m_update[iU - 1] - j0;
    // alternating batches to make optimal use of the workspace
    static constexpr index_t w1b[]{2, 1, 2, 1};
    static constexpr index_t w2b[]{3, 1, 2, 1};
    auto W   = work_update.middle_cols(j0, nj);
    auto UpL = W.batch(l & 3);
    auto Σ   = work_update_Σ.batch(0).middle_rows(j0, nj);
    auto WQ  = work_hyh.batch(iU);
    hyhound_diag_apply(cr_U.batch(iU), W.batch((l + w1b[i & 3]) & 3), //
                       W.batch((l + w2b[i & 3]) & 3),                 //
                       UpL, Σ, WQ, 0, jsplit);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_Y(index_t l, index_t iY) {
    if constexpr (VL == 1)
        if (iY + (1 << l) >= p) // Y(iY)=0 for scalar case
            return;
    GUANAQO_TRACE("Update Y", iY);
    const index_t offset = 1 << l, i = iY >> (l + 1),
                  j0 = iY == offset ? 0 : m_update[iY - 1 - offset],
                  j1 = m_update[std::min(iY - 1 + offset, p - 1)], nj = j1 - j0,
                  jsplit = m_update[iY - 1] - j0;
    // alternating batches to make optimal use of the workspace
    static constexpr index_t w1b[]{1, 2, 1, 2};
    static constexpr index_t w2b[]{1, 2, 1, 3};
    auto W   = work_update.middle_cols(j0, nj);
    auto UpL = W.batch(l & 3);
    auto Σ   = work_update_Σ.batch(0).middle_rows(j0, nj);
    auto WQ  = work_hyh.batch(iY);
    hyhound_diag_apply(cr_Y.batch(iY), W.batch((l + w1b[i & 3]) & 3), //
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
                            pcr_U.batch(l), WU, WU, Σ, ml);
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
    update_riccati(ctx, ΔΣ);
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

// Algorithm 3 “Factorization update of a single modified Riccati block column”
//
// Differences compared to the pseudo-code in the paper:
//  - Many operations are performed in-place to reduce memory usage.
//    For example, all original Cholesky factors are replaced by the updated ones.
//  - The workspaces Υ1 and Υ2 are reused for the variables Υ and Φ in the paper. Two workspaces
//    are required because the matrix multiplication by Φx(j) cannot be done in-place.
//  - Only the constraints for which ΔΣ is nonzero are used during the update. This is done by
//    compressing the relevant columns of Dᵀ and Cᵀ into Υu and Υx respectively.
//  - A global communication step is used at the end to compute the total update rank for the entire
//    problem, and to partition the workspace for Υ˃ and Υ˂ to prepare for the CR phase.

template <index_t VL, class T, StorageOrder DefaultOrder>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder>::update_riccati(Context &ctx, view<> ΔΣ) {
    const index_t c = ctx.index;
    //  3|  j₁ = n(c-1)+1, jₙ = nc
    const index_t dn  = c * n; // data batch index
    const index_t jn  = c * n; // stage index
    const index_t nux = nu + nx, nyM = std::max(ny, ny_0 + ny_N);
    // TODO: special case nyM for c == 0
    auto LHs = riccati_LH.batch(c);
    auto B̂s = riccati_LAB.batch(c).right_cols(n * nu), Âs = riccati_LAB.batch(c).left_cols(n * nx);
    auto Υ1 = riccati_Υ1.batch(c), Υ2 = riccati_Υ2.batch(c);
    auto 𝑆 = work_Σ.batch(c); // mathcal{S}_j in the paper

    index_t m = 0; // Total update rank so far
    {
        GUANAQO_TRACE("Riccati update compress", jn);
        //  4|  [ Υu(jₙ) ]   [ D(jₙ)ᵀ ]
        //   |  [ Υx(jₙ) ] = [ C(jₙ)ᵀ ],    𝑆(jₙ) = ΔΣ(jₙ)
        //   |  [ Υλ(jₙ) ]   [   0    ]
        //  6|  m(j) = rank 𝑆(j)
        // Note that we only need to consider the columns corresponding to changing constraints,
        // i.e. where ΔΣ is nonzero, which is why we compress them.
        auto Υ_first = Υ2.left_cols(nyM);
        auto Υux     = Υ_first.top_rows(nu + nx); // we don't know the number of columns yet
        m            = compress_masks(data_Gᵀ.batch(dn), ΔΣ.batch(dn), Υux, 𝑆.top_rows(nyM));
        auto Υλ      = Υ_first.bottom_left(nx, m);
        Υλ.set_constant(0);
    }

    // Iterate over all stages in the interval (in reverse order)
    for (index_t i = 0; i < n; ++i) {
        //  5|  for j = jₙ downto j₁
        index_t j = sub_wrap_N(jn, i);
        auto LH = LHs.middle_cols(i * nux, nux), LRS = LH.left_cols(nu);
        auto LQ = tril(LH.bottom_right(nx, nx));
        auto LB = B̂s.middle_cols(i * nu, nu), Acl = Âs.middle_cols(i * nx, nx);

        index_t mj = m;
        auto Υ     = (i & 1 ? Υ1 : Υ2).left_cols(mj); // alternate between Υ1 and Υ2 workspaces
        auto Υux = Υ.top_rows(nu + nx), Υλ = Υ.bottom_rows(nx);
        if (mj > 0) {
            GUANAQO_TRACE("Riccati update R", j);
            //  7|  [ L̃R(j)    0   ]   [ LR(j)  Υu(j) ]
            //   |  [ L̃S(j)  Φx(j) ] = [ LS(j)  Υx(j) ] Q̆u(j),  blkdiag(I, 𝑆(j))-orthogonal
            //   |  [ L̃B(j)  Φλ(j) ]   [ LB(j)  Υλ(j) ]
            hyhound_diag_2(tril(LRS), Υux, //
                           LB, Υλ, 𝑆.top_rows(mj));
        }
        auto Φx = Υ.middle_rows(nu, nx), Φλ = Υ.bottom_rows(nx);
        //  8|  if j > j₁
        if (i + 1 < n) {
            [[maybe_unused]] const auto j_next = sub_wrap_N(j, 1);
            const auto di_next                 = dn + i + 1;
            auto Υ_next                        = (i & 1 ? Υ2 : Υ1).left_cols(mj + nyM);
            auto Υux_next = Υ_next.top_rows(nu + nx), Υλ_next = Υ_next.bottom_rows(nx);
            auto F_next = data_F.batch(di_next);
            if (mj > 0) {
                GUANAQO_TRACE("Riccati update prop", j_next);
                // 10|  [ Υu(j-1) ]   [ B(j-1)ᵀ Φx(j)   D(j-1)ᵀ ]
                //   |  [ Υx(j-1) ] = [ A(j-1)ᵀ Φx(j)   C(j-1)ᵀ ]
                //   |  [ Υλ(j-1) ]   [    Φλ(j)          0     ]
                // Left block column first
                gemm(F_next.transposed(), Φx, Υux_next.left_cols(mj));
                copy(Φλ, Υλ_next.left_cols(mj));
                // TODO: we may not have to copy Φλ every time. In fact, we can already write it in
                //       the CR workspace.
            }
            {
                GUANAQO_TRACE("Riccati update compress", j_next);
                // Now the right block column, again compressing to only the changing constraints
                m += compress_masks(data_Gᵀ.batch(di_next), ΔΣ.batch(di_next),
                                    Υux_next.right_cols(nyM), 𝑆.middle_rows(mj, nyM));
                Υλ_next.middle_cols(mj, m - mj).set_constant(0);
            }
            if (mj > 0) {
                GUANAQO_TRACE("Riccati update Q", j);
                //  9|  Ãcl(j) = Acl(j) + Φλ(j) 𝑆(j) Φx(j)ᵀ
                gemm_diag_add(Φλ, Φx.transposed(), Acl, 𝑆.top_rows(mj));
                // 12|  [ L̃Q(j)  0 ] = [ LQ(j)  Φx(j) ] Q̆x(j),  blkdiag(I, 𝑆(j))-orthogonal
                hyhound_diag(LQ, Φx, 𝑆.top_rows(mj));
            }
        } else {
            const auto c_prev = sub_wrap_p(c, 1); // c-1
            // Communicate the update ranks mj to all threads and compute the partial sums (i.e. the
            // column offsets in the global update workspace we'll write Υ(c) and Υ(c-1) to)
            m_update[c_prev] = mj;
            ctx.run_single_sync(
                [this] { std::inclusive_scan(begin(m_update), end(m_update), begin(m_update)); });
            if (mj > 0) {
                GUANAQO_TRACE("Riccati update Q", j);
                auto Tc    = LH.block(nu - 1, nu, nx, nx); // T(c) = LQ(j₁)⁻ᵀ, see compute_schur
                auto Υ_fwd = work_Ups_fwd(0, c), Υ_bwd_prev = work_Ups_bwd(0, c_prev);
                auto 𝒮cr = work_Σ_cr(0, c); // mathscr{S}_c in the paper
                // 12|  [ L̃Q(j)  0 ] = [ LQ(j)  Φx(j) ] Q̆x(j),  blkdiag(I, 𝑆(j))-orthogonal
                // Fused with:
                // 14|  [ L̃A(j₁)  Υ˃(c)   ] = [ LA(j₁)  Φλ(j₁) ] Q̆x(j₁),
                //   |  [ -T̃(c)   Υ˂(c-1) ]   [ -T(c)     0    ]
                hyhound_diag_riccati(LQ, Φx,                  //
                                     Acl, Φλ, Υ_fwd,          //
                                     Tc, /*0*/ Υ_bwd_prev,    // note the lack of a minus sign ...
                                     𝑆.top_rows(mj), c == 0); //
                compact_blas::xneg(simdify(Υ_bwd_prev));      // which is fixed here (TODO: fuse)
                // 13|  𝒮(c) = 𝑆(j₁)
                c == 0 ? compact_blas::template xadd_neg_copy<-1>(simdify(𝒮cr),
                                                                  simdify(𝑆.top_rows(mj)))
                       : compact_blas::template xadd_neg_copy<+0>(simdify(𝒮cr),
                                                                  simdify(𝑆.top_rows(mj)));
                // We negate 𝒮(c) because in the CR update, we need blkdiag(-I, 𝒮(c))-orthogonal
                // or blkdiag(I, -𝒮(c))-orthogonal transformations.
            }
        }
    }
}

} // namespace CYQLONE_NS(cyqlone)::v2
