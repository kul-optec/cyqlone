#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/loop.hpp>

#include <batmat/linalg/compress.hpp>
#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/trsm.hpp>
#include <batmat/linalg/trtri.hpp>
#include <utility>

#ifndef CYQLONE_FACTOR_DO_PREFETCH
#define CYQLONE_FACTOR_DO_PREFETCH 0
#endif

namespace CYQLONE_NS(cyqlone) {

using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_Y([[maybe_unused]] index_t l, index_t biY) {
    GUANAQO_TRACE("Trsm Y", biY);
    trsm(coupling_Y.batch(biY), tril(coupling_D.batch(biY)).transposed());
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_K(index_t l, index_t bi) {
    const index_t bi_prev = sub_wrap_PmV(bi, 1 << l), bi_next = add_wrap_PmV(bi, 1 << l);
#if CYQLONE_FACTOR_DO_PREFETCH
    for (index_t c = 0; c < coupling_U.cols(); c += 1)
        for (index_t r = 0; r < coupling_U.rows(); r += 16)
            __builtin_prefetch(&coupling_U.batch(bi)(0, r, c), 0, 3);
#endif
    // Compute UYᵀ or YUᵀ
    if (ν2p(bi_prev) > ν2p(bi_next)) {
        GUANAQO_TRACE("Compute U", bi_next);
        gemm_neg(coupling_U.batch(bi), coupling_Y.batch(bi).transposed(),
                 coupling_U.batch(bi_next));
    } else {
        GUANAQO_TRACE("Compute Y", bi_prev);
        gemm_neg(coupling_Y.batch(bi), coupling_U.batch(bi).transposed(),
                 coupling_Y.batch(bi_prev));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_U([[maybe_unused]] index_t l, index_t biU) {
    GUANAQO_TRACE("Trsm U", biU);
    trsm(coupling_U.batch(biU), tril(coupling_D.batch(biU)).transposed());
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_L(index_t l, index_t bi) {
    const index_t offset = 1 << l;
    const index_t biU    = add_wrap_PmV(bi, offset);
    const index_t biY    = sub_wrap_PmV(bi, offset);
#if CYQLONE_FACTOR_DO_PREFETCH
    for (index_t c = 0; c < coupling_Y.cols(); c += 1)
        for (index_t r = 0; r < coupling_Y.rows(); r += 16)
            __builtin_prefetch(&coupling_Y.batch(biY)(0, r, c), 0, 3);
#endif
    { // D -= UUᵀ
        GUANAQO_TRACE("Subtract UUᵀ", bi);
        syrk_sub(coupling_U.batch(biU), tril(coupling_D.batch(bi)));
    }
    if (ν2p(bi) == l + 1 && bi != 0) { // chol(D - YYᵀ)
        GUANAQO_TRACE("Factor D", bi);
        syrk_sub_potrf(coupling_Y.batch(biY), tril(coupling_D.batch(bi)));
    } else { // D -= YYᵀ
        GUANAQO_TRACE("Subtract YYᵀ", bi);
        bi == 0 ? syrk_sub(coupling_Y.batch(biY), tril(coupling_D.batch(bi)), with_rotate_C<1>,
                           with_rotate_D<1>, with_mask_D<1>)
                : syrk_sub(coupling_Y.batch(biY), tril(coupling_D.batch(bi)));
    }
    // chol(D)
    if (ν2p(bi) == l + 1 && bi == 0) {
        GUANAQO_TRACE("Factor D", bi);
        potrf(tril(coupling_D.batch(bi)), tril(pcr_L.batch(0)));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_pcr() {
    [this]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template factor_pcr_level<Levels>(), ...);
    }(std::make_integer_sequence<index_t, CyqloneSolver::lvl>{});
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void CyqloneSolver<VL, T, DefaultOrder>::factor_pcr_level() {
    GUANAQO_TRACE("Factor PCR", Level);
    static constexpr auto stride = 1 << Level;
    auto A                       = Level == 0 ? coupling_D.batch(0) : pcr_A.batch(0);
    auto B                       = Level == 0 ? coupling_Y.batch(0) : pcr_Y.batch(Level);
    auto A_next                  = pcr_A.batch(0);
    trsm(B.transposed(), triu(pcr_L.batch(Level).transposed()), pcr_U.batch(Level),
         with_shift_A<-stride>);
    trsm(B, triu(pcr_L.batch(Level).transposed()), pcr_Y.batch(Level));
    syrk_sub(pcr_U.batch(Level), tril(A), tril(A_next), with_rotate_C<-stride>,
             with_rotate_D<-stride>, with_mask_D<-stride>);
    syrk_sub(pcr_Y.batch(Level), tril(A_next), with_rotate_C<+stride>, with_rotate_D<+stride>,
             with_mask_D<+stride>);
    potrf(tril(A_next), tril(pcr_L.batch(Level + 1)));
    if constexpr (Level + 1 < lvl) {
        auto B_next = pcr_Y.batch(Level + 1);
        gemm_neg(pcr_Y.batch(Level), pcr_U.batch(Level).transposed(), B_next, {},
                 with_rotate_C<-stride>, with_rotate_D<-stride>, with_mask_D<-stride>);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Solve>
void CyqloneSolver<VL, T, DefaultOrder>::factor_l0_solve(Context &ctx, mut_view<> ux,
                                                         mut_view<> λ) {
    const index_t ti         = ctx.index;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    const index_t biI        = sub_wrap_PmV(ti, 1);
    const index_t biA        = ti;
    const auto biR           = biA;
    const auto diI           = biI * num_stages;
    const auto diA           = biA * num_stages;
    const bool x_lanes       = biA == 0; // first stage wraps around
    // Coupling equation to previous stage is eliminated after coupling
    // equation to next stage for odd threads, vice versa for even threads.
    const bool I_below_A = (biA & 1) == 1;
    // Update the subdiagonal blocks U and Y of the coupling equations
    auto DiI = tril(coupling_D.batch(biI));
    auto DiA = tril(coupling_D.batch(biA));
    auto Âi  = riccati_ÂB̂.batch(biR).middle_cols(nx * (num_stages - 1), nx);
    auto ÂB̂i = riccati_ÂB̂.batch(biR).right_cols(nx + nu * num_stages);
    auto R̂ŜQ̂ = riccati_R̂ŜQ̂.batch(biR);
    auto Q̂i  = tril(R̂ŜQ̂.bottom_right(nx, nx));
    // LQ⁻ᵀ is upper triangular, stored one row up from LQ itself
    assert(nu >= 1);
    auto Q̂i_inv = triu(R̂ŜQ̂.right_cols(nx).middle_rows(nu - 1, nx));
    {
        GUANAQO_TRACE("Invert Q", biI);
        trtri(Q̂i, Q̂i_inv.transposed());
    }
    if (I_below_A) {
        // Top block is A → column index is row index of A (biA)
        // Target block in cyclic part is U in column λ(kA)
        GUANAQO_TRACE("Compute first U", biA);
        trmm_neg(Q̂i_inv, Âi.transposed(), coupling_U.batch(biA));
    } else {
        // Top block is I → column index is row index of I (biI)
        // Target block in cyclic part is Y in column λ(kI)
        GUANAQO_TRACE("Compute first Y", biI);
        x_lanes ? trmm_neg(Âi, Q̂i_inv.transposed(), coupling_Y.batch(biI), with_rotate_C<-1>,
                           with_rotate_D<-1>, with_mask_D<-1>)
                : trmm_neg(Âi, Q̂i_inv.transposed(), coupling_Y.batch(biI));
    }
    // Each column of the cyclic part with coupling equations is updated by
    // two threads: one for the forward, and one for the backward coupling.
    // Update the diagonal blocks of the coupling equations,
    // first forward in time ...
    {
        GUANAQO_TRACE("Compute L⁻ᵀL⁻¹", biI);
        x_lanes ? trmm(Q̂i_inv, Q̂i_inv.transposed(), DiI, with_rotate_C<-1>, with_rotate_D<-1>,
                       with_mask_D<-1>)
                : trmm(Q̂i_inv, Q̂i_inv.transposed(), DiI);
    }
    // Then synchronize to make sure there are no two threads updating the
    // same diagonal block.
    ctx.arrive_and_wait();
    // And finally backward in time, optionally merged with factorization.
    if (lP == lvl) {
        GUANAQO_TRACE("Factor D last", biA);
        syrk_add(ÂB̂i, DiA);
        potrf(DiA, tril(pcr_L.batch(0)));
    } else if (ν2p(biA) == 0) {
        GUANAQO_TRACE("Factor D", biA);
        syrk_add_potrf(ÂB̂i, DiA);
    } else {
        GUANAQO_TRACE("Compute (BA)(BA)ᵀ", biA);
        syrk_add(ÂB̂i, DiA);
    }
    if constexpr (Solve) {
        auto tok = ctx.arrive();
        {
            GUANAQO_TRACE("Update λ", diI);
            auto x_last = ux.batch(diA + num_stages - 1).bottom_rows(nx);
            x_lanes ? compact_blas::template xsub<-1>(simdify(λ.batch(diI)), simdify(x_last))
                    : compact_blas::template xsub<+0>(simdify(λ.batch(diI)), simdify(x_last));
        }
        ctx.wait(std::move(tok));
        {
            GUANAQO_TRACE("Solve λ", diI);
            if (ν2p(biI) == 0)
                trsm(DiI, λ.batch(diI));
        }
    }
}

// Performs Riccati recursion and then factors level l=0 of
// coupling equations + propagates the subdiagonal blocks to level l=1.
template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Solve>
void CyqloneSolver<VL, T, DefaultOrder>::factor_riccati_solve(Context &ctx, value_type S, view<> Σ,
                                                              mut_view<> ux, mut_view<> λ) {
    constexpr bool alt = true; // Don't store intermediate BA LQ products
    using batmat::linalg::compress_masks_sqrt;
    const index_t ti         = ctx.index;
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux = nu + nx, nyM = std::max(ny, ny_0 + ny_N);
    auto R̂ŜQ̂       = riccati_R̂ŜQ̂.batch(ti);
    auto B̂         = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â         = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    auto BADCᵀ     = riccati_BAᵀ.batch(ti);
    auto A0        = data_BA.batch(di0).right_cols(nx);
    auto DCᵀ0      = data_DCᵀ.batch(di0);
    index_t m_syrk = 0;
    // Copy B and A from the last stage
    {
        GUANAQO_TRACE("Riccati init", k0);
        copy(data_BA.batch(di0).left_cols(nu), B̂.left_cols(nu));
        m_syrk = compress_masks_sqrt(DCᵀ0, Σ.batch(di0), BADCᵀ.left_cols(nyM));
    }
    for (index_t i = 0; i < num_stages; ++i) {
        const index_t k = sub_wrap_N(k0, i);
        const auto di   = di0 + i;
        auto R̂ŜQ̂i       = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto R̂Ŝi        = R̂ŜQ̂i.left_cols(nu);
        auto R̂i         = R̂Ŝi.top_rows(nu);
        auto Ŝi         = R̂Ŝi.bottom_rows(nx);
        auto Q̂i         = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i         = B̂.middle_cols(i * nu, nu);
        auto Âi         = Â.middle_cols(i * nx, nx);
        auto BADCᵀ_prev = BADCᵀ.middle_cols(alt || i == 0 ? 0 : (i - 1) * nx, m_syrk);
        {
            GUANAQO_TRACE("Riccati QRS", k);
            using std::isfinite;
            // Factor R̂, update Ŝ, factor Q̂
            syrk_add_potrf(BADCᵀ_prev, tril(data_RSQ.batch(di)), tril(R̂ŜQ̂i), 1 / S);
            if constexpr (Solve) {
                // Solve u ← LR̂⁻¹ u, x ← x - Ŝ u
                auto ui = ux.batch(di).top_rows(nu), xi = ux.batch(di).bottom_rows(nx);
                trsm(tril(R̂i), ui);
                gemv_sub(Ŝi, ui, xi);
            }
            // Compute LB̂ = B̂ LR̂⁻ᵀ
            trsm(B̂i, tril(R̂i).transposed());
            if constexpr (Solve) {
                auto ui = ux.batch(di).top_rows(nu), λ_last = λ.batch(di0);
                gemv_add(B̂i, ui, λ_last);
            }
            // Update Â = Ã - LB̂ LŜᵀ
            i == 0 ? gemm_sub(B̂i, Ŝi.transposed(), A0, Âi) //
                   : gemm_sub(B̂i, Ŝi.transposed(), Âi);
        }
        if (i + 1 < num_stages) {
            // Copy next B and A
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            GUANAQO_TRACE("Riccati update AB", k_next);
            const auto di_next = di0 + i + 1;
            auto BADCᵀ_next    = BADCᵀ.middle_cols(alt ? 0 : i * nx, nx + nyM);
            auto BAᵀ_next = BADCᵀ_next.left_cols(nx), DCᵀ_next = BADCᵀ_next.right_cols(nyM);
            auto BAi = data_BA.batch(di_next);
            auto Bi = BAi.left_cols(nu), Ai = BAi.right_cols(nx);
            // Compute next B̂ and Â
            auto B̂_next = B̂.middle_cols((i + 1) * nu, nu);
            auto Â_next = Â.middle_cols((i + 1) * nx, nx);
            gemm(Âi, Bi, B̂_next);
            gemm(Âi, Ai, Â_next);
            if constexpr (Solve) {
                auto xi = ux.batch(di).bottom_rows(nx), ux_next = ux.batch(di_next),
                     λ_next = λ.batch(di_next), λ_last = λ.batch(di0);
                gemv_add(Âi, λ_next, λ_last); // λ(jn) += Â λ(j-1)
                auto w = work_cr.batch(ti).left_cols(1);
                trmm(tril(Q̂i).transposed(), λ_next, w); // w = LQᵀ(j) λ(j-1)
                trmm(tril(Q̂i), w);                      // w = LQ(j) LQᵀ(j) λ(j-1)
                compact_blas::xsub_copy(simdify(w), simdify(xi),
                                        simdify(w));    // w = x(j) - LQ(j) LQᵀ(j) λ(j-1)
                gemv_add(BAi.transposed(), w, ux_next); // u(j-1) += BAᵀ(j-1) w
            }
            // Riccati update
            trmm(BAi.transposed(), tril(Q̂i), BAᵀ_next);
            // TODO: merge with next potrf
            m_syrk = nx + compress_masks_sqrt(data_DCᵀ.batch(di_next), Σ.batch(di_next), DCᵀ_next);
        } else {
            // Compute LÂ = Ã LQ⁻ᵀ
            GUANAQO_TRACE("Riccati last", k);
            trsm(Âi, tril(Q̂i).transposed());
            if constexpr (Solve) {
                auto xi = ux.batch(di).bottom_rows(nx), λ_last = λ.batch(di0);
                trsm(tril(Q̂i), xi);
                gemv_add(Âi, xi, λ_last);
                trsm(tril(Q̂i).transposed(), xi);
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Solve>
void CyqloneSolver<VL, T, DefaultOrder>::factor_solve_impl(Context &ctx, value_type S, view<> Σ,
                                                           mut_view<> ux, mut_view<> λ) {
    index_t ti = ctx.index;
    factor_riccati_solve<Solve>(ctx, S, Σ, ux, λ);
    factor_l0_solve<Solve>(ctx, ux, λ);
    for (index_t l = 0; l < lP - lvl; ++l) {
        ctx.arrive_and_wait(); // Wait for L
        const auto biU = add_wrap_PmV(ti, 1), biY = sub_wrap_PmV(ti, (1 << l) - 1);
        if (ν2p(biU) == l) {
            factor_U(l, biU);
            if constexpr (Solve)
                solve_u_forward(l, biU, λ);
        } else if (ν2p(biY) == l) {
            factor_Y(l, biY);
            if constexpr (Solve)
                solve_y_forward(l, biY, λ, work_cr);
        }
        ctx.arrive_and_wait(); // Wait for U, Y
        if (ν2p(biU) == l) {
            factor_L(l, biY);
            if constexpr (Solve)
                solve_λ_forward(l, biY, λ, work_cr);
        } else if (ν2p(biY) == l) {
            update_K(l, biY);
        }
    }

    if (solve_method == SolveMethod::PCR) {
        ctx.arrive_and_wait(); // wait for off-diagonal block
        if (ν2p(ti + 1) + 1 == lP - lvl)
            factor_pcr();
    }

    if constexpr (Solve) {
        if (ν2p(ti + 1) + 1 == lP - lvl) {
            if (solve_method == SolveMethod::PCR)
                solve_pcr(λ.batch(0), work_pcg.batch(0).left_cols(1));
            else
                solve_pcg(λ.batch(0), work_pcg.batch(0));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_solve(Context &ctx, value_type S, view<> Σ,
                                                      mut_view<> ux, mut_view<> λ) {
    factor_solve_impl<true>(ctx, S, Σ, ux, λ);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor(Context &ctx, value_type S, view<> Σ) {
    factor_solve_impl<false>(ctx, S, Σ, {}, {});
}

} // namespace CYQLONE_NS(cyqlone)
