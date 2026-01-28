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

// Algorithm 4 “Cyqlone factorization updates”
//
// Differences compared to the pseudo-code in the paper:
//   - The update of the last has been modified to allow for vectorization (v>1), updating the
//     PCR factorization if necessary.
//   - A heuristic rank check is used to decide whether to update or re-factorize the last level.
//   - The update matrices Y˃(0) are skipped when they are zero (i.e. when the updates to u(0) are
//     handled separately). This saves some unnecessary computation in the scalar case.

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_L(index_t l, index_t i) {
    if (l < lp()) {
        GUANAQO_TRACE("Update L", i);
        auto L   = tril(cr_L.batch(i));
        auto UpQ = work_Q_cr(l, i);
        auto Σ   = work_Σ_Q(l, i);
        auto WQ  = work_hyh.batch(i);
        // 16|  [ L̃(i) | 0 ] = [ L(i) | Υ˃(i)  Υ˂(i) ] Q̆(i),  blkdiag(-I, 𝒮(i;l+1))-orthogonal
        hyhound_diag(L, UpQ, Σ, WQ);
        return;
    }

    // Last level
    auto M0 = tril(cr_L.batch(0)), L0 = tril(pcr_L.batch(0));
    auto Y0   = cr_Y.batch(0);
    auto Ypen = cr_Y.batch(p / 2), Upen = cr_U.batch(p / 2); // Subdiag blocks of penultimate level

    if (m_update_u0 > 0)
        m_update[p - 1] += m_update_u0; // Make room for the updates from D(0)
    auto Υ0_bwd = work_Ups_bwd(l, 0), Υ0_fwd = work_Ups_fwd(l, 0);
    auto Σ_bwd = work_Σ_bwd(l, 0), Σ_fwd = work_Σ_fwd(l, 0);
    BATMAT_ASSERT(Σ_bwd.rows() == Σ_fwd.rows() || m_update_u0 > 0);
    // Include contributions from D(0) if needed
    if (m_update_u0 >= 0) {
        BATMAT_ASSERT(VL == 1); // handling D(0) separately is only possible in the scalar case
        // Υ˃(0) = 0, so no forward update
        Υ0_fwd.reassign(Υ0_fwd.left_cols(0));
        Σ_fwd.reassign(Σ_fwd.top_rows(0));
        // Copy the update contributions from D(0) in the rightmost columns of Υ0_bwd
        auto Υ2  = riccati_Υ2.batch(0);
        auto Φλ0 = Υ2.bottom_right(nx, ny_0).left_cols(m_update_u0);
        auto 𝑆   = work_Σ.batch(0);
        auto 𝑆u0 = 𝑆.bottom_rows(ny_0).top_rows(m_update_u0);
        compact_blas::xadd_neg_copy(simdify(Σ_bwd.bottom_rows(m_update_u0)), simdify(𝑆u0));
        copy(Φλ0, Υ0_bwd.right_cols(m_update_u0));
    }

    // For p=2, v=4, the update of the last level looks like:
    //
    // [ Υ˂(0)                Υ˃(0) | L(0)                   ]
    // [ Υ˃(2)  Υ˂(2)               | Y(0)  L(2)             ]
    // [        Υ˃(4)  Υ˂(4)        |       Y(2)  L(4)       ]
    // [               Υ˃(6)  Υ˂(6) |             Y(4)  L(6) ]
    //
    // where the blocks are stored as follows:
    //  Υ0_bwd = [ Υ˂(0)  Υ˂(2)  Υ˂(4)  Υ˂(6) ]
    //  Υ0_fwd = [ Υ˃(2)  Υ˃(4)  Υ˃(6)  Υ˃(0) ]
    //  L0     = [ L(0)   L(2)   L(4)   L(6) ]
    //  Y0     = [ Y(0)   Y(2)   Y(4)   -    ]
    //
    // Note that Υ˂ and Υ˃ are aligned by column, not by row. To apply the updates (row-wise),
    // we therefore need to rotate Υ0_fwd by one block to the right first.

    // Check the rank to decide whether to update or recompute
    const index_t nj      = std::max(Σ_fwd.rows(), Σ_bwd.rows());
    auto pcr_update_thres = params.pcr_max_update_fraction * static_cast<double>(nx);
    auto y0_update_thres  = params.cr_max_update_fraction_Y0 * static_cast<double>(nx);
    bool update           = static_cast<double>(nj) < pcr_update_thres;
    bool update_y         = static_cast<double>(nj) < y0_update_thres;
    bool do_update_pcr    = params.solve_method == SolveMethod::PCR && update && VL > 1;
    bool do_refactor_pcr  = params.solve_method == SolveMethod::PCR && !update;

    // Perform the PCR update
    if (do_update_pcr)
        update_pcr(Υ0_fwd, Υ0_bwd, Σ_bwd);

    { // Update or recompute the matrices Y(0), M(0) and L(0) in the last CR level
        GUANAQO_TRACE("Update L", i);
        // Update or recompute the subdiagonal block Y of the last CR level.
        // If there's only a single thread, we always update because there is no previous CR level
        // to recompute from (we would need to recompute the Riccati products, which is slow).
        // Otherwise, we only update if the rank is sufficiently low.
        if constexpr (VL > 1) {
            if (update_y || p == 1)
                gemm_diag_add(Υ0_fwd, Υ0_bwd.transposed(), Y0, Σ_fwd);
            else
                gemm_neg(Ypen, Upen.transposed(), Y0);
        }
        // If at some point in the future we need to refactor PCR, we may need Y(0). So we just
        // always update it here. Alternatively, we could recompute it when needed, but that would
        // complicate the bookkeeping. Besides, we need Y(0) for the PCG case anyway.

        // Make sure the diagonal block M of the last CR level is up to date (it is needed for PCR).
        // This is done in two steps, the backward and the forward updates, the latter of which
        // requires a rotation first.
        if (params.solve_method == SolveMethod::PCR)
            syrk_diag_add(Υ0_bwd, M0, Σ_bwd);
        // When using PCG, we need the Cholesky factors L(0) of M(0) for the preconditioner, so
        // update them here. Like with the update of M(0), we do this in two steps.
        if (!do_update_pcr)
            hyhound_diag(L0, Υ0_bwd, Σ_bwd);
        // Rotate and repeat for the forward update.
        batmat::linalg::copy(Σ_fwd, Σ_fwd, with_rotate<-1>);
        batmat::linalg::copy(Υ0_fwd, Υ0_fwd, with_rotate<-1>);
        if (params.solve_method == SolveMethod::PCR)
            syrk_diag_add(Υ0_fwd, M0, Σ_fwd);
        if (!do_update_pcr)
            hyhound_diag(L0, Υ0_fwd, Σ_fwd);
        // TODO: we should actually merge these two hyhound_diag calls to make sure that the
        //       intermediate matrix after the backward update does not become indefinite
        //       (although this shouldn't be an issue for QPALM, at least not in exact arithmetic).
        //       We already have the code for this in update_pcr_level.
    }

    // Finally, recompute the PCR factorization if we did not do an update.
    if (do_refactor_pcr)
        factor_pcr();
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_U(index_t l, index_t i) {
    GUANAQO_TRACE("Update U", i);
    const index_t i_bwd = sub_wrap_ceil_p(i, 1 << l);
    auto Up_bwd = work_Ups_bwd(l, i_bwd), Up_bwd_next = work_Ups_bwd(l + 1, i_bwd);
    if constexpr (VL == 1)
        if (i >= p) { // happens in cases where p is not a power of two
            // There's no matrix Q̆(i) to apply, just copy the update matrices forward
            if (Up_bwd.data != Up_bwd_next.data)
                copy(Up_bwd, Up_bwd_next);
            // If the number of threads is odd, then update_Y won't be called for this column i,
            // so we need to copy the forward update matrices here as well.
            index_t i_fwd = add_wrap_ceil_p(i, 1 << l);
            if (i_fwd >= p)
                i_fwd = 0;
            if (i_fwd == 0 && m_update_u0 >= 0)
                return; // Υ˃(0) = 0
            auto Up_fwd = work_Ups_fwd(l, i_fwd), Up_fwd_next = work_Ups_fwd(l + 1, i_fwd);
            if (Up_fwd.data != Up_fwd_next.data)
                copy(Up_fwd, Up_fwd_next);
            return;
        }
    auto UpQ = work_Q_cr(l, i);
    auto Σ   = work_Σ_Q(l, i);
    auto WQ  = work_hyh.batch(i);
    auto U   = cr_U.batch(i);
    // 18|  [ Ũ(i) | Υ˂(i-2^l;l+1) ] = [ U(i) | Υ˂(i-2^l;l)  0 ] Q̆(i)
    hyhound_diag_apply(U, Up_bwd, Up_bwd_next, //
                       UpQ, Σ, WQ, 0);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_Y(index_t l, index_t i) {
    GUANAQO_TRACE("Update Y", i);
    index_t i_fwd = add_wrap_ceil_p(i, 1 << l);
    if (i_fwd >= p)
        i_fwd = 0;
    if (i_fwd == 0 && m_update_u0 >= 0)
        return; // Υ˃(0) = 0
    auto UpQ    = work_Q_cr(l, i);
    auto Σ      = work_Σ_Q(l, i);
    auto WQ     = work_hyh.batch(i);
    auto Y      = cr_Y.batch(i);
    auto Up_fwd = work_Ups_fwd(l, i_fwd), Up_fwd_next = work_Ups_fwd(l + 1, i_fwd);
    // 20|  [ Ỹ(i) | Υ˃(i+2^l;l+1) ] = [ Y(i) | 0  Υ˃(i+2^l;l) ] Q̆(i)
    hyhound_diag_apply(Y, Up_fwd, Up_fwd_next, //
                       UpQ, Σ, WQ, Up_fwd_next.cols() - Up_fwd.cols());
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
    //  WL = [ Υ˃(0)  | Υ˂(0)  ]
    //  WY = [   0    | Υ˃(+1) ]
    //  WU = [ Υ˂(-1) |   0    ]
    batmat::linalg::copy(Σ.top_rows(ml), Σ.bottom_rows(ml), with_rotate<+rot0>);
    batmat::linalg::copy(Σ.top_rows(ml), Σ.top_rows(ml), with_rotate<-rot1>);
    if constexpr (l < lv()) {
        auto WL = work_update_pcr_L.left_cols(2 * ml).batch(0);
        auto WU = WYU.right_cols(VL * m).left_cols(2 * ml);
        auto WY = WYU.left_cols(VL * m).right_cols(2 * ml);
        // Note that [ WY WU ] is contiguous (although this does not really help us since they have
        // different rotations)
        batmat::linalg::copy(WY.right_cols(ml), WL.left_cols(ml), with_rotate<-rot1>);
        batmat::linalg::copy(WU.left_cols(ml), WL.right_cols(ml), with_rotate<+rot0>);
        batmat::linalg::copy(WU, WU, with_rotate<-rot1>); // TODO: fuse with hyhound_diag_cyclic
        batmat::linalg::copy(WY, WY, with_rotate<+rot0>);
        hyhound_diag_cyclic(tril(pcr_L.batch(l)), WL,              //
                            pcr_Y.batch(l), WY.right_cols(ml), WY, //
                            pcr_U.batch(l), WU.left_cols(ml), WU, Σ);
    } else {
        batmat::linalg::copy(WYU, WYU, with_rotate<rot0>); // TODO: fuse with hyhound_diag
        hyhound_diag(tril(pcr_L.batch(l)), WYU, Σ);
    }
    // TODO: Can we exploit the complementary sparsity patterns of Υ˃(0) and Υ˂(0) in the last level
    //       of PCR? Right now, this is only done for the scalar case (v=1).
}

// TODO: write down the pseudocode for this algorithm in the appendix of the paper?
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_pcr(batch_view<> fwd, batch_view<> bwd,
                                                    batch_view<> Σbwd) {
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
    batmat::linalg::copy(Σbwd, Σ.top_rows(m));
    [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template update_pcr_level<Levels>(m, WYU, Σ), ...);
    }(std::make_integer_sequence<index_t, CyqloneSolver::lv() + 1>{});
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update(Context &ctx, view<> ΔΣ) {
    const index_t c = ctx.index;
    //  2|  Υ˃(c;0), Υ˂(c-1;0), 𝒮(c;0) = update-block-column-riccati(c)
    //  3|  update-schur(c)
    update_riccati(ctx, ΔΣ);
    //  5|  -- sync --
    ctx.arrive_and_wait(); // wait for Υ˃, Υ˂
    //  6|  if ν₂(c) = 0:  update-L(0, c)
    if (ν2p(c) == 0)
        update_L(0, c);
    //  7|  for l = 0 ... log₂(P)-1
    for (index_t l = 0; l < lp(); ++l) {
        const auto c_ = cr_thread_assignment(l, c);
        //  8|  iU = c+1, iY = c+1-2^l
        const auto iU = add_wrap_ceil_p(c_, 1), iY = sub_wrap_ceil_p(c_, (1 << l) - 1);
        //  9|  -- sync --
        ctx.arrive_and_wait(); // wait for Q̆
        // 10|  if ν₂(iU) = l:  update-U(l, iU)
        if (ν2p(iU) == l)
            update_U(l, iU);
        // 11|  elif ν₂(iY) = l:  update-Y(l, iY)
        else if (ν2p(iY) == l)
            update_Y(l, iY);
        // 12|  -- sync --
        ctx.arrive_and_wait(); // wait for Υ˃, Υ˂
        // 13|  if ν₂(iY) = l+1:  update-L(l+1, iY)
        if (ν2p(iY) == l + 1)
            update_L(l + 1, iY);
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
//  - The update for u(0) is handled as a special case to exploit its mostly independent structure.
//  - If the number of processors p is not a power of two, the workspace allocation of Υ˃(0) needs
//    to be adjusted to ensure that it does not overlap with Υ˂(p-2^l). Note that this is only
//    necessary when u(0) is not isolated. See work_Ups_fwd_w.
//  - In the vectorized case, Υ˃(0) and Υ˂(0) are stored in different workspaces in the last level
//    of CR, since this is not actually the last level of the full reduction (PCR handles the rest).
//    See work_Ups_bwd_w.

template <index_t VL, class T, StorageOrder DefaultOrder>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder>::update_riccati(Context &ctx, view<> ΔΣ) {
    const index_t c = ctx.index;
    //  3|  j₁ = n(c-1)+1, jₙ = nc
    const index_t dn  = c * n; // data batch index
    const index_t jn  = c * n; // stage index
    const index_t nux = nu + nx, nyM = std::max(ny, ny_0 + ny_N);
    auto LHs = riccati_LH.batch(c);
    auto B̂s = riccati_LAB.batch(c).right_cols(n * nu), Âs = riccati_LAB.batch(c).left_cols(n * nx);
    auto Υ1 = riccati_Υ1.batch(c), Υ2 = riccati_Υ2.batch(c);
    auto 𝑆 = work_Σ.batch(c); // \mathcal{S}_j in the paper

    // u(0) is mostly independent, since there is no coupling S(0) or A(0). Without vectorization
    // (v=1), we can handle it as a special case. This not only saves computation during the Riccati
    // update, but also introduces structural zeros that can be exploited during the CR updates.
    // Its contribution just has to be applied to LB(0) (which is done in this function), and to
    // M(0)/L(0) (which is done in update_L).
    const bool isolate_u0 = VL == 1 && dn == 0;

    index_t m    = 0; // Total update rank so far
    index_t mu0  = 0; // Update rank for u(0)
    auto Υ_first = Υ2.left_cols(nyM), Υu0_first = Υ2.right_cols(ny_0);
    if (!isolate_u0) {
        GUANAQO_TRACE("Riccati update compress", jn);
        //  4|  [ Υu(jₙ) ]   [ D(jₙ)ᵀ ]
        //   |  [ Υx(jₙ) ] = [ C(jₙ)ᵀ ],    𝑆(jₙ) = ΔΣ(jₙ)
        //   |  [ Υλ(jₙ) ]   [   0    ]
        //  6|  m(j) = rank 𝑆(j)
        // Note that we only need to consider the columns corresponding to changing constraints,
        // i.e. where ΔΣ is nonzero, which is why we compress them.
        auto Υux = Υ_first.top_rows(nu + nx); // we don't know the number of columns yet
        m        = compress_masks(data_Gᵀ.batch(dn), ΔΣ.batch(dn), //
                                  Υux, 𝑆.top_rows(nyM));
        auto Υλ  = Υ_first.bottom_left(nx, m);
        Υλ.set_constant(0);
    } else {
        // Exploit the block-diagonal structure of G₀ = [ D₀ 0 ]  ny_0
        //                                              [ 0  Cₙ]  ny_N
        auto D0ᵀ = data_Gᵀ.batch(dn).top_left(nu, ny_0),
             C0ᵀ = data_Gᵀ.batch(dn).bottom_rows(nx).middle_cols(ny_0, ny_N);
        auto Υu0 = Υu0_first.top_rows(nu), Υx = Υ_first.middle_rows(nu, nx).left_cols(ny_N);
        mu0     = compress_masks(D0ᵀ, ΔΣ.batch(dn).top_rows(ny_0), //
                                 Υu0, 𝑆.bottom_rows(ny_0));
        m       = compress_masks(C0ᵀ, ΔΣ.batch(dn).middle_rows(ny_0, ny_N), //
                                 Υx, 𝑆.top_rows(ny_N));
        auto Υλ = Υ_first.bottom_left(nx, m), Υλ0 = Υu0_first.bottom_left(nx, mu0);
        Υλ.set_constant(0);
        Υλ0.set_constant(0);
    }
    auto Υu0 = Υu0_first.top_left(nu, mu0), Υλ0 = Υu0_first.bottom_left(nx, mu0);
    auto 𝑆u0 = 𝑆.bottom_rows(ny_0).top_rows(mu0);

    // Iterate over all stages in the interval (in reverse order)
    for (index_t i = 0; i < n; ++i) {
        //  5|  for j = jₙ downto j₁
        index_t j = sub_wrap_N(jn, i);
        auto LH = LHs.middle_cols(i * nux, nux), LRS = LH.left_cols(nu);
        auto LR = tril(LRS.top_rows(nu)), LQ = tril(LH.bottom_right(nx, nx));
        auto LB = B̂s.middle_cols(i * nu, nu), Acl = Âs.middle_cols(i * nx, nx);

        index_t mj = m;
        auto Υ     = (i & 1 ? Υ1 : Υ2).left_cols(mj); // alternate between Υ1 and Υ2 workspaces
        auto Υux = Υ.top_rows(nu + nx), Υλ = Υ.bottom_rows(nx);
        if (!isolate_u0 || i != 0) {
            GUANAQO_TRACE("Riccati update RS", j);
            if (mj > 0)
                //  7|  [ L̃R(j)    0   ]   [ LR(j)  Υu(j) ]
                //   |  [ L̃S(j)  Φx(j) ] = [ LS(j)  Υx(j) ] Q̆u(j),  blkdiag(I, 𝑆(j))-orthogonal
                //   |  [ L̃B(j)  Φλ(j) ]   [ LB(j)  Υλ(j) ]
                hyhound_diag_2(tril(LRS), Υux, //
                               LB, Υλ, 𝑆.top_rows(mj));
        } else {
            GUANAQO_TRACE("Riccati update R", j);
            if (mu0 > 0)
                // Same as above, but using LS(j) = 0 = L̃S(j), Υx(j) = 0 = Φx(j)
                hyhound_diag_2(LR, Υu0, //
                               LB, Υλ0, 𝑆u0);
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
#ifndef NDEBUG
            if (ctx.is_master())
                work_update.set_constant(std::numeric_limits<T>::quiet_NaN());
#endif
            const auto c_prev = sub_wrap_p(c, 1); // c-1
            // Communicate the update ranks mj to all threads and compute the partial sums (i.e. the
            // column offsets in the global update workspace we'll write Υ(c) and Υ(c-1) to)
            m_update[c_prev] = mj;
            if (dn == 0)
                m_update_u0 = isolate_u0 ? mu0 : -1;
            ctx.run_single_sync(
                [this] { std::inclusive_scan(begin(m_update), end(m_update), begin(m_update)); });
            const index_t i_fwd = c, i_bwd = c_prev;
            const bool rot = c == 0;
            if (mj > 0) {
                GUANAQO_TRACE("Riccati update Q", j);
                auto Tc    = LH.block(nu - 1, nu, nx, nx); // T(c) = LQ(j₁)⁻ᵀ, see compute_schur
                auto Υ_fwd = work_Ups_fwd(0, i_fwd).left_cols(mj),
                     Υ_bwd_prev = work_Ups_bwd(0, i_bwd).left_cols(mj);
                auto 𝒮cr        = work_Σ_fwd(0, i_fwd).top_rows(mj); // \mathscr{S}_c in the paper
                // 12|  [ L̃Q(j)  0 ] = [ LQ(j)  Φx(j) ] Q̆x(j),  blkdiag(I, 𝑆(j))-orthogonal
                // Fused with:
                // 14|  [ L̃A(j₁)  Υ˃(c)   ] = [ LA(j₁)  Φλ(j₁) ] Q̆x(j₁),
                //   |  [ -T̃(c)   Υ˂(c-1) ]   [ -T(c)     0    ]
                hyhound_diag_riccati(LQ, Φx,               //
                                     Acl, Φλ, Υ_fwd,       //
                                     Tc, /*0*/ Υ_bwd_prev, // note the lack of a minus sign ...
                                     𝑆.top_rows(mj), rot); //
                compact_blas::xneg(simdify(Υ_bwd_prev));   // which is fixed here (TODO: fuse)
                // 13|  𝒮(c) = 𝑆(j₁)
                rot ? compact_blas::template xadd_neg_copy<-1>(simdify(𝒮cr),
                                                               simdify(𝑆.top_rows(mj)))
                    : compact_blas::template xadd_neg_copy<+0>(simdify(𝒮cr),
                                                               simdify(𝑆.top_rows(mj)));
                // We negate 𝒮(c) because in the CR update, we need blkdiag(-I, 𝒮(c))-orthogonal
                // or blkdiag(I, -𝒮(c))-orthogonal transformations.
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] std::pair<index_t, index_t>
CyqloneSolver<VL, T, DefaultOrder>::cols_Ups_fwd(index_t l, index_t i) const {
    BATMAT_ASSUME(ν2p(i) >= l); // i % offset = 0
    const index_t offset = 1 << l, floor_mask = offset - 1;
    // Current block ends at i (or at p if i == 0),
    // minus one because m_update is an inclusive sum.
    const index_t ip  = i == 0 ? p : i;
    const index_t end = m_update[ip - 1];
    // Current block starts at the previous multiple of offset.
    const index_t i_start = (ip - 1) & ~floor_mask;
    const index_t start   = i_start > 0 ? m_update[i_start - 1] : 0;
    return {start, end};
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] std::pair<index_t, index_t>
CyqloneSolver<VL, T, DefaultOrder>::cols_Ups_bwd(index_t l, index_t i) const {
    BATMAT_ASSUME(ν2p(i) >= l); // i % offset = 0
    const index_t offset = 1 << l;
    // The start index of the next block (at i + offset),
    // minus one because m_update is an inclusive sum.
    // If p is not a power of two, we need to clamp to p.
    const index_t i_end = std::min(i + offset, p);
    const index_t end   = m_update[i_end - 1];
    // The start index of the current block is i.
    const index_t start = i > 0 ? m_update[i - 1] : 0;
    return {start, end};
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] std::pair<index_t, index_t>
CyqloneSolver<VL, T, DefaultOrder>::cols_Q_cr(index_t l, index_t i) const {
    return {cols_Ups_fwd(l, i).first, cols_Ups_bwd(l, i).second};
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] index_t CyqloneSolver<VL, T, DefaultOrder>::work_Ups_fwd_w(index_t l,
                                                                         index_t i) const {
    const index_t offset = 1 << l, floor_mask = offset - 1;
    if (i == 0 && l + 2 <= lp()) {
        i = (p - 1) & ~floor_mask; // beginning of the last block
        i += offset;               // make sure we don't overlap with it
    }
    return i == 0 ? l + 2 : std::min(l + 2, ν2(i));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] index_t CyqloneSolver<VL, T, DefaultOrder>::work_Ups_bwd_w(index_t l,
                                                                         index_t i) const {
    if (l == lp())
        return l; // Keep Υ˃(0) @ [l+2] and Υ˂(0) @ [l] in separate workspaces at the last level
    return i == 0 ? l + 2 : std::min(l + 2, ν2(i));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] auto CyqloneSolver<VL, T, DefaultOrder>::work_Ups_fwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_fwd(l, i);
    index_t w         = work_Ups_fwd_w(l, i);
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] auto CyqloneSolver<VL, T, DefaultOrder>::work_Ups_bwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_bwd(l, i);
    const index_t w   = work_Ups_bwd_w(l, i);
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] auto CyqloneSolver<VL, T, DefaultOrder>::work_Q_cr(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Q_cr(l, i);
    const index_t w   = l;
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] auto CyqloneSolver<VL, T, DefaultOrder>::work_Σ_fwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_fwd(l, i);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] auto CyqloneSolver<VL, T, DefaultOrder>::work_Σ_bwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_bwd(l, i);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
[[nodiscard]] auto CyqloneSolver<VL, T, DefaultOrder>::work_Σ_Q(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Q_cr(l, i);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

} // namespace CYQLONE_NS(cyqlone)
