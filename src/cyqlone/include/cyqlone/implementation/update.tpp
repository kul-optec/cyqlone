#include <cyqlone/cyqlone.hpp>
#include <cyqlone/tracing.hpp>

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

//! [Cyqlone update CR helper]
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_L(index_t l, index_t i) {
    if (l < lp()) {
        CYQ_TRACE_READ(Upf, i, 0);
        CYQ_TRACE_READ(Upb, i, 0);
        GUANAQO_TRACE("Update L", i);
        CYQ_TRACE_WRITE(Q, i, 0);
        CYQ_TRACE_WRITE(Q, i, 1);
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

    auto Υ0_bwd = work_Ups_bwd_last(), Υ0_fwd = work_Ups_fwd_last();
    auto Σ_bwd = work_Σ_bwd_last(), Σ_fwd = work_Σ_fwd_last();
    BATMAT_ASSERT(Σ_bwd.rows() == Σ_fwd.rows() || m_update_u0 >= 0);

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
    auto pcr_update_thres = params.pcr_max_update_fraction * static_cast<double>(block_size);
    auto y0_update_thres  = params.cr_max_update_fraction_Y0 * static_cast<double>(block_size);
    bool update           = static_cast<double>(nj) < pcr_update_thres;
    bool update_y         = static_cast<double>(nj) < y0_update_thres;
    bool do_update_pcr    = params.solve_method == SolveMethod::PCR && update && VL > 1;
    bool do_refactor_pcr  = params.solve_method == SolveMethod::PCR && !update;

    CYQ_TRACE_READ(Upf, 0, 0);
    CYQ_TRACE_READ(Upb, 0, 0);
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
        factor_pcr(); // TODO: use parallel variant (when doing so, synchronize in update_solve_cr)
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_U(index_t l, index_t i) {
    const index_t i_bwd = sub_wrap_ceil_p(i, 1 << l);
    CYQ_TRACE_READ(Upb, i_bwd, 0);
    CYQ_TRACE_READ(Q, i, 1);
    GUANAQO_TRACE("Update U", i);
    CYQ_TRACE_WRITE(Upb, i_bwd, 0);
    auto Up_bwd = work_Ups_bwd(l, i_bwd), Up_bwd_next = work_Ups_bwd(l + 1, i_bwd);
    if constexpr (VL == 1)
        if (i >= p) { // happens in cases where p is not a power of two
            // There's no matrix Q̆(i) to apply, just copy the update matrices forward
            if (Up_bwd.data() != Up_bwd_next.data())
                copy(Up_bwd, Up_bwd_next);
            // If the number of threads is odd, then update_Y won't be called for this column i,
            // so we need to copy the forward update matrices here as well.
            index_t i_fwd = add_wrap_ceil_p(i, 1 << l);
            if (i_fwd >= p)
                i_fwd = 0;
            if (i_fwd == 0 && m_update_u0 >= 0)
                return; // Υ˃(0) = 0
            auto Up_fwd = work_Ups_fwd(l, i_fwd), Up_fwd_next = work_Ups_fwd(l + 1, i_fwd);
            if (Up_fwd.data() != Up_fwd_next.data())
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

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_Y(index_t l, index_t i) {
    index_t i_fwd = add_wrap_ceil_p(i, 1 << l);
    CYQ_TRACE_READ(Upf, i_fwd, 0);
    CYQ_TRACE_READ(Q, i, 0);
    GUANAQO_TRACE("Update Y", i);
    CYQ_TRACE_WRITE(Upf, i_fwd, 0);
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
//! [Cyqlone update CR helper]

//! [PCR update]
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
template <index_t Level>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_pcr_level(index_t m, mut_batch_view<> WYU,
                                                                mut_batch_view<> WΣ) {
    constexpr index_t l = Level;
    // The algorithm requires the update matrices that are not reduced in the current level to be
    // offset by 2^l. We could do this by first rotating them by 2^l, applying the Householder
    // transformations, and then rotating them back. However, this would be inefficient, so instead
    // we leave the workspace rotated by 2^l from the previous level, and adjust the rotations in
    // the next level.
    constexpr index_t rot = 1 << l, prev_rot = rot >> 1;
    const index_t ml = m << l;
    GUANAQO_TRACE("Update PCR", l);
    auto Σ = WΣ.bottom_rows(2 * ml);
    if constexpr (prev_rot != 0)
        batmat::linalg::copy(Σ.bottom_rows(ml), Σ.bottom_rows(ml), with_rotate<+prev_rot>);
    batmat::linalg::copy(Σ.bottom_rows(ml), Σ.top_rows(ml), with_rotate<-rot>);
    if constexpr (l + 1 < lv()) {
        //          S(-1)    S(0)
        //  WL = [ Υ˃(0)  | Υ˂(0)  ]
        //  WY = [   0    | Υ˃(+1) ]
        //  WU = [ Υ˂(-1) |   0    ]
        auto WL  = work_update_pcr_L.left_cols(2 * ml).batch(0);
        auto WU0 = WYU.right_cols(VL * m / 2).left_cols(2 * ml);
        auto W0Y = WYU.left_cols(VL * m / 2).right_cols(2 * ml);
        auto WY  = W0Y.right_cols(ml);
        auto WU  = WU0.left_cols(ml);
        // undo workspace rotation
        batmat::linalg::copy(WY, WL.left_cols(ml), with_rotate<-prev_rot>);
        batmat::linalg::copy(WU, WL.right_cols(ml), with_rotate<+prev_rot>);
        // rotate element k-2^l to position k (but the workspace is already at -prev_rot)
        batmat::linalg::copy(WU, WU, with_rotate<-rot + prev_rot>);
        // rotate element k+2^l to position k (but the workspace is already at +prev_rot)
        batmat::linalg::copy(WY, WY, with_rotate<+rot - prev_rot>);
        // [ L̃(k;l) |       0       ]   [ L(k;l) | Υ˃(k;l)      Υ˂(k;l)     ]
        // [ Ũ(k;l) | Υ˂(k-2^l;l+1) ] = [ U(k;l) | Υ˂(k-2^l;l)     0        ] Q̆(k;l)
        // [ Ỹ(k;l) | Υ˃(k+2^l;l+1) ] = [ Y(k;l) |    0         Υ˃(k+2^l;l) ]
        hyhound_diag_cyclic(tril(pcr_L.batch(l)), WL, //
                            pcr_Y.batch(l), WY, W0Y,  //
                            pcr_U.batch(l), WU, WU0, Σ);
    } else {
        auto WL = WYU;
        auto WU = work_update_pcr_L.left_cols(2 * ml).batch(0);
        // undo workspace rotation
        batmat::linalg::copy(WYU.left_cols(ml), WL.left_cols(ml), with_rotate<-prev_rot>);
        batmat::linalg::copy(WYU.right_cols(ml), WL.right_cols(ml), with_rotate<+prev_rot>);
        //           S(-1)    S(0)
        //  WL =  [ Υ˃(0)  | Υ˂(0)  ]
        //  WYU = [ Υ˃(+1) | Υ˂(-1) |
        // rotate element k±2^l to position k
        batmat::linalg::copy(WL.left_cols(ml), WU.right_cols(ml), with_rotate<rot>);
        batmat::linalg::copy(WL.right_cols(ml), WU.left_cols(ml), with_rotate<rot>);
        // [ L̃(k;l) |       0       ]   [ L(k;l) | Υ˃(k;l)      Υ˂(k;l)     ]
        // [ Ũ(k;l) | Υ˂(k-2^l;l+1) ] = [ U(k;l) | Υ˂(k-2^l;l)  Υ˃(k+2^l;l) ] Q̆(k;l)
        hyhound_diag_2(tril(pcr_L.batch(l)), WL, pcr_U.batch(l), WU, Σ);
        batmat::linalg::copy(WU, WU, with_rotate<rot>); // undo rotation
        batmat::linalg::copy(Σ, Σ, with_rotate<+rot>);
        // Final diagonal block
        // [ L̃(k;l+1) |   0   ] = [ L(k;l+1) | Υ˃(k;l+1)  Υ˂(k;l+1) ] Q̆(k;l+1)
        hyhound_diag(tril(pcr_L.batch(l + 1)), WU, Σ);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_pcr(batch_view<> fwd, batch_view<> bwd,
                                                          batch_view<> Σbwd) {
    index_t m = fwd.cols();
    BATMAT_ASSUME(m == bwd.cols());
    auto WYU = work_update_pcr_UY.left_cols(VL * m).batch(0);
    auto WY  = WYU.left_cols(VL * m / 2); // WY and WU start in the middle of WYU and grow outwards
    auto WU  = WYU.right_cols(VL * m / 2);
    auto Σ   = work_update_pcr_Σ.top_rows(VL * m).batch(0);
    batmat::linalg::copy(bwd, WU.left_cols(m));
    batmat::linalg::copy(fwd, WY.right_cols(m), with_rotate<-1>);
    batmat::linalg::copy(Σbwd, Σ.bottom_rows(m));
    [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template update_pcr_level<Levels>(m, WYU, Σ), ...);
    }(std::make_integer_sequence<index_t, TricyqleSolver::lv()>{});
}
//! [PCR update]

//! [Cyqlone update]
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
template <bool Solve>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::update_solve_impl(Context &ctx, view<> ΔΣ,
                                                                mut_view<> ux, mut_view<> λ) {
    //  2|  Υ˃(c;0), Υ˂(c-1;0), 𝒮(c;0) = update-block-column-riccati(c)
    //  3|  update-schur(c)
    update_riccati_solve<Solve>(ctx, ΔΣ, ux, λ);
    //  5|  -- sync --
    ctx.arrive_and_wait(); // wait for Υ˃, Υ˂, x_next
    if constexpr (Solve) {
        const index_t c   = ctx.index; // different assignment than compute_schur
        const auto c_next = add_wrap_p(c, 1);
        const auto dn = c * n, dn_next = c_next * n, d1_next = dn_next + n - 1; // see compute_schur
        auto x_next = ux.batch(d1_next).bottom_rows(nx);
        c_next > 0 || v == 1 ? sub(λ.batch(dn), x_next) //
                             : sub(λ.batch(dn), x_next, with_rotate<1>);
    }
    // Update the block-tridiagonal Schur complement using CR
    tricyqle.template update_solve_cr<Solve>(ctx, λ, n);
}
//! [Cyqlone update]

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::update(Context &ctx, view<> ΔΣ) {
    update_solve_impl<false>(ctx, ΔΣ, {}, {});
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::update_solve(Context &ctx, view<> ΔΣ, mut_view<> ux,
                                                           mut_view<> λ) {
    update_solve_impl<true>(ctx, ΔΣ, ux, λ);
}

//! [Cyqlone update CR]
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
template <bool Solve>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_solve_cr(Context &ctx, mut_view<> λ,
                                                               index_t stride) {
    const index_t c = ctx.index;
    //  6|  if ν₂(c) = 0:  update-L(0, c)
    if (ν2p(c) == 0) {
        update_L(0, c);
        if constexpr (Solve)
            if (p != 1)
                trsm(tril(cr_L.batch(c)), λ.batch(c * stride));
    }
    //  7|  for l = 0 ... log₂(P)-1
    for (index_t l = 0; l < lp(); ++l) {
        const auto c_ = cr_thread_assignment(l, c);
        //  8|  iU = c+1, iY = c+1-2^l
        const auto iU = add_wrap_ceil_p(c_, 1), iY = sub_wrap_ceil_p(c_, (1 << l) - 1);
        //  9|  -- sync --
        ctx.arrive_and_wait(); // wait for Q̆
        // 10|  if ν₂(iU) = l:  update-U(l, iU)
        if (ν2p(iU) == l) {
            update_U(l, iU);
            if constexpr (Solve)
                solve_u_forward(l, iU, λ, stride);
        }
        // 11|  elif ν₂(iY) = l:  update-Y(l, iY)
        else if (ν2p(iY) == l) {
            update_Y(l, iY);
            if constexpr (Solve)
                solve_y_forward(l, iY, λ, work_cr, stride);
        }
        // 12|  -- sync --
        ctx.arrive_and_wait(); // wait for Υ˃, Υ˂
        // 13|  if ν₂(iY) = l+1:  update-L(l+1, iY)
        if (ν2p(iY) == l + 1)
            update_L(l + 1, iY);
        if (ν2p(iU) == l)
            if constexpr (Solve)
                solve_λ_forward(l, iY, λ, work_cr, stride);
    }
    if constexpr (Solve) {
        ctx.arrive_and_wait();
        // TODO: synchronize here if switching to parallel PCR factor in update_L
        if (ν2p(c + 1) + 1 == lp() || p == 1)
            params.solve_method == SolveMethod::PCR
                ? solve_pcr(λ.batch(0), work_pcg.batch(0).left_cols(1))
                : solve_pcg(λ.batch(0), work_pcg.batch(0));
    }
}
//! [Cyqlone update CR]

// Algorithm 3 “Factorization update of a single modified Riccati block column”

//! [Cyqlone update Riccati]
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
template <bool Solve>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::update_riccati_solve(Context &ctx, view<> ΔΣ,
                                                                   mut_view<> ux, mut_view<> λ) {
    const index_t c = riccati_thread_assignment(ctx);
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
        if (nyM > 0)
            m = compress_masks(data_Gᵀ.batch(dn), ΔΣ.batch(dn), //
                               Υux, 𝑆.top_rows(nyM));
        auto Υλ = Υ_first.bottom_left(nx, m);
        Υλ.set_constant(0);
    } else {
        // Exploit the block-diagonal structure of G₀ = [ D₀ 0 ]  ny_0
        //                                              [ 0  Cₙ]  ny_N
        auto D0ᵀ = data_Gᵀ.batch(dn).top_left(nu, ny_0),
             C0ᵀ = data_Gᵀ.batch(dn).bottom_rows(nx).middle_cols(ny_0, ny_N);
        auto Υu0 = Υu0_first.top_rows(nu), Υx = Υ_first.middle_rows(nu, nx).left_cols(ny_N);
        if (ny_0 > 0)
            mu0 = compress_masks(D0ᵀ, ΔΣ.batch(dn).top_rows(ny_0), //
                                 Υu0, 𝑆.bottom_rows(ny_0));
        if (ny_N > 0)
            m = compress_masks(C0ᵀ, ΔΣ.batch(dn).middle_rows(ny_0, ny_N), //
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
        const index_t j  = sub_wrap_ceil_N(jn, i); // stage index j ≡ jₙ - i mod N
        const index_t di = dn + i;                 // data batch index
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
        if constexpr (Solve) {
            // Solve u ← LR̂⁻¹ u, x ← x - Ŝ u
            auto ui = ux.batch(di).top_rows(nu), xi = ux.batch(di).bottom_rows(nx);
            trsm(LR, ui);
            auto S = LRS.bottom_rows(nx);
            gemv_sub(S, ui, xi);
            auto λ_last = λ.batch(dn);
            gemv_add(LB, ui, λ_last);
        }
        //  8|  if j > j₁
        if (i + 1 < n) {
            [[maybe_unused]] const auto j_next = sub_wrap_ceil_N(j, 1);
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
                if (nyM > 0)
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
            if constexpr (Solve) {
                auto xi = ux.batch(di).bottom_rows(nx), ux_next = ux.batch(di_next),
                     λ_next = λ.batch(di_next), λ_last = λ.batch(dn);
                gemv_add(Acl, λ_next, λ_last); // λ(jn) += Â λ(j-1)
                auto w = tricyqle.work_cr.batch(c).left_cols(1);
                trmm(LQ.transposed(), λ_next, w);          // w = LQᵀ(j) λ(j-1)
                trmm(LQ, w);                               // w = LQ(j) LQᵀ(j) λ(j-1)
                sub(xi, w, w);                             // w = x(j) - LQ(j) LQᵀ(j) λ(j-1)
                gemv_add(F_next.transposed(), w, ux_next); // u(j-1) += BAᵀ(j-1) w
            }
        } else {
#ifndef NDEBUG
            if (ctx.is_master())
                tricyqle.work_update.set_constant(std::numeric_limits<T>::quiet_NaN());
#endif
            const auto c_prev = sub_wrap_p(c, 1); // c-1
            // Communicate the update ranks mj to all threads and compute the column offsets in the
            // global update workspace we'll write Υ(c) and Υ(c-1) to.
            tricyqle.set_thread_update_rank(ctx, c_prev, mj);
            const index_t i_fwd = c, i_bwd = c_prev;
            const bool rot = c == 0;
            GUANAQO_TRACE("Riccati update Q", j);
            CYQ_TRACE_WRITE(Upf, i_fwd, 0);
            CYQ_TRACE_WRITE(Upb, i_bwd, 0);
            if (mj > 0) {
                auto Tc    = LH.block(nu - 1, nu, nx, nx); // T(c) = LQ(j₁)⁻ᵀ, see compute_schur
                auto Υ_fwd = tricyqle.work_Ups_fwd(0, i_fwd).left_cols(mj),
                     Υ_bwd_prev = tricyqle.work_Ups_bwd(0, i_bwd).left_cols(mj);
                auto 𝒮cr = tricyqle.work_Σ_fwd(0, i_fwd).top_rows(mj); // \mathscr{S}_c in the paper
                // 12|  [ L̃Q(j)  0 ] = [ LQ(j)  Φx(j) ] Q̆x(j),  blkdiag(I, 𝑆(j))-orthogonal
                // Fused with:
                // 14|  [ L̃A(j₁)  Υ˃(c)   ] = [ LA(j₁)  Φλ(j₁) ] Q̆x(j₁),
                //   |  [ -T̃(c)   Υ˂(c-1) ]   [ -T(c)     0    ]
                hyhound_diag_riccati(LQ, Φx,               //
                                     Acl, Φλ, Υ_fwd,       //
                                     Tc, /*0*/ Υ_bwd_prev, // note the lack of a minus sign ...
                                     𝑆.top_rows(mj), rot); //
                negate(Υ_bwd_prev);                        // which is fixed here (TODO: fuse)
                // 13|  𝒮(c) = 𝑆(j₁)
                rot ? negate(𝑆.top_rows(mj), 𝒮cr, with_rotate<1>) //
                    : negate(𝑆.top_rows(mj), 𝒮cr);
                // We negate 𝒮(c) because in the CR update, we need blkdiag(-I, 𝒮(c))-orthogonal
                // or blkdiag(I, -𝒮(c))-orthogonal transformations.
            }
            if constexpr (Solve) {
                auto xi = ux.batch(di).bottom_rows(nx), λ_last = λ.batch(dn);
                trsm(LQ, xi);
                gemv_add(Acl, xi, λ_last);
                trsm(LQ.transposed(), xi);
            }
            if (dn == 0) {
                // Add the contribution from the isolated update for u(0) as well
                if (isolate_u0) {
                    tricyqle.set_update_rank_extra(mu0);
                    copy(Υλ0, tricyqle.work_Ups_extra());
                    negate(𝑆u0, tricyqle.work_Σ_extra());
                } else {
                    tricyqle.clear_update_rank_extra();
                }
            }
        }
    }
}
//! [Cyqlone update Riccati]

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::set_thread_update_rank(Context &ctx, index_t c,
                                                                      index_t m) {
    m_update[c] = m;
    ctx.run_single_sync(
        [this] { std::inclusive_scan(begin(m_update), end(m_update), begin(m_update)); });
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::set_update_rank_extra(index_t m) {
    m_update_u0 = m;
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::clear_update_rank_extra() {
    m_update_u0 = -1;
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] std::pair<index_t, index_t>
TricyqleSolver<VL, T, DefaultOrder, Ctx>::cols_Ups_fwd(index_t l, index_t i) const {
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

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] std::pair<index_t, index_t>
TricyqleSolver<VL, T, DefaultOrder, Ctx>::cols_Ups_bwd(index_t l, index_t i) const {
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

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] std::pair<index_t, index_t>
TricyqleSolver<VL, T, DefaultOrder, Ctx>::cols_Q_cr(index_t l, index_t i) const {
    return {cols_Ups_fwd(l, i).first, cols_Ups_bwd(l, i).second};
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] index_t TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_fwd_w(index_t l,
                                                                               index_t i) const {
    const index_t offset = 1 << l, floor_mask = offset - 1;
    if (i == 0 && l + 2 <= lp()) {
        i = (p - 1) & ~floor_mask; // beginning of the last block
        i += offset;               // make sure we don't overlap with it
    }
    return i == 0 ? l + 2 : std::min(l + 2, ν2(i));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] index_t TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_bwd_w(index_t l,
                                                                               index_t i) const {
    if (l == lp())
        return l; // Keep Υ˃(0) @ [l+2] and Υ˂(0) @ [l] in separate workspaces at the last level
    return i == 0 ? l + 2 : std::min(l + 2, ν2(i));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_fwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_fwd(l, i);
    index_t w         = work_Ups_fwd_w(l, i);
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_bwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_bwd(l, i);
    const index_t w   = work_Ups_bwd_w(l, i);
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Q_cr(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Q_cr(l, i);
    const index_t w   = l;
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Σ_fwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_fwd(l, i);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Σ_bwd(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Ups_bwd(l, i);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Σ_Q(index_t l, index_t i)
    -> mut_batch_view<column_major> {
    auto [start, end] = cols_Q_cr(l, i);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_fwd_last()
    -> mut_batch_view<column_major> {
    const index_t l = lp(), i = 0;
    auto [start, end] = cols_Ups_fwd(l, i);
    index_t w         = work_Ups_fwd_w(l, i);
    if (m_update_u0 >= 0)
        return work_update.batch(w & 3).middle_cols(start, 0);
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_bwd_last()
    -> mut_batch_view<column_major> {
    const index_t l = lp(), i = 0;
    auto [start, end] = cols_Ups_bwd(l, i);
    const index_t w   = work_Ups_bwd_w(l, i);
    if (m_update_u0 >= 0)
        end += m_update_u0; // include extra columns in Υ˂(0) in the last level
    return work_update.batch(w & 3).middle_cols(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Σ_fwd_last()
    -> mut_batch_view<column_major> {
    const index_t l = lp(), i = 0;
    auto [start, end] = cols_Ups_fwd(l, i);
    if (m_update_u0 >= 0)
        return work_update_Σ.batch(0).middle_rows(start, 0);
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Σ_bwd_last()
    -> mut_batch_view<column_major> {
    const index_t l = lp(), i = 0;
    auto [start, end] = cols_Ups_bwd(l, i);
    if (m_update_u0 >= 0)
        end += m_update_u0;
    return work_update_Σ.batch(0).middle_rows(start, end - start);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Ups_extra()
    -> mut_batch_view<column_major> {
    BATMAT_ASSERT(m_update_u0 >= 0);
    return work_Ups_bwd_last().right_cols(m_update_u0);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
[[nodiscard]] auto TricyqleSolver<VL, T, DefaultOrder, Ctx>::work_Σ_extra()
    -> mut_batch_view<column_major> {
    BATMAT_ASSERT(m_update_u0 >= 0);
    return work_Σ_bwd_last().bottom_rows(m_update_u0);
}

} // namespace CYQLONE_NS(cyqlone)
