#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/loop.hpp>

#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/trsm.hpp>
#include <batmat/linalg/trtri.hpp>

namespace cyqlone {

using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_schur_Y(index_t l, index_t biY) {
    const index_t offset = 1 << l;
    { // Compute Y[bi]
        GUANAQO_TRACE("Trsm Y", biY);
        trsm(coupling_Y.batch(biY), tril(coupling_D.batch(biY)).transposed());
    }
    // Wait for U[bi] from factor_schur_U
    barrier();
    for (index_t c = 0; c < coupling_U.cols(); c += 1)
        for (index_t r = 0; r < coupling_U.rows(); r += 16)
            __builtin_prefetch(&coupling_U.batch(biY)(0, r, c), 0, 3);
    // Compute UYᵀ or YUᵀ
    if (is_U_below_Y(l, biY)) {
        const index_t bi_next = add_wrap_PmV(biY, offset); // TODO: need mod?
        GUANAQO_TRACE("Compute U", bi_next);
        gemm_neg(coupling_U.batch(biY), coupling_Y.batch(biY).transposed(),
                 coupling_U.batch(bi_next));
    } else {
        const index_t bi_prev = sub_wrap_PmV(biY, offset); // TODO: need mod?
        GUANAQO_TRACE("Compute Y", bi_prev);
        gemm_neg(coupling_Y.batch(biY), coupling_U.batch(biY).transposed(),
                 coupling_Y.batch(bi_prev));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_schur_U(index_t l, index_t biU) {
    const index_t offset = 1 << l;
    const index_t biD    = sub_wrap_PmV(biU, offset);
    const index_t biY    = sub_wrap_PmV(biD, offset);
    for (index_t c = 0; c < coupling_D.cols(); c += 1)
        for (index_t r = 0; r < coupling_D.rows(); r += 16)
            __builtin_prefetch(&coupling_D.batch(biD)(0, r, c), 0, 3);
    { // Compute U[bi]
        GUANAQO_TRACE("Trsm U", biU);
        trsm(coupling_U.batch(biU), tril(coupling_D.batch(biU)).transposed());
    }
    // Wait for Y[bi] from factor_schur_Y
    barrier();
    for (index_t c = 0; c < coupling_Y.cols(); c += 1)
        for (index_t r = 0; r < coupling_Y.rows(); r += 16)
            __builtin_prefetch(&coupling_Y.batch(biY)(0, r, c), 0, 3);
    { // D -= UUᵀ
        GUANAQO_TRACE("Subtract UUᵀ", biD);
        syrk_sub(coupling_U.batch(biU), tril(coupling_D.batch(biD)));
    }
    if (is_active(l + 1, biD)) { // chol(D - YYᵀ)
        BATMAT_ASSUME(biD != 0);
        GUANAQO_TRACE("Factor D", biD);
        syrk_sub_potrf(coupling_Y.batch(biY), tril(coupling_D.batch(biD)));
    } else { // D -= YYᵀ
        GUANAQO_TRACE("Subtract YYᵀ", biD);
        biD == 0 ? syrk_sub(coupling_Y.batch(biY), tril(coupling_D.batch(biD)), with_rotate_C<1>,
                            with_rotate_D<1>, with_mask_D<1>)
                 : syrk_sub(coupling_Y.batch(biY), tril(coupling_D.batch(biD)));
    }
    // chol(D)
    if (l + 1 == lP - lvl && biD == 0) {
        GUANAQO_TRACE("Factor D", biD);
        potrf(tril(coupling_D.batch(biD)));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_l0(const index_t ti) {
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    const index_t biI        = sub_wrap_PmV(ti, 1);
    const index_t biA        = ti;
    const auto biR           = biA;
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
    barrier();
    // And finally backward in time, optionally merged with factorization.
    const bool do_factor = (biA & 1) == 1 || (lP - lvl == 0 && biA == 0);
    if (do_factor) {
        GUANAQO_TRACE("Factor D", biA);
        syrk_add_potrf(ÂB̂i, DiA);
    } else {
        GUANAQO_TRACE("Compute (BA)(BA)ᵀ", biA);
        syrk_add(ÂB̂i, DiA);
    }
}

// Performs Riccati recursion and then factors level l=0 of
// coupling equations + propagates the subdiagonal blocks to level l=1.
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_riccati(index_t ti, bool alt, value_type S,
                                                        view<> Σ) {
    const index_t num_stages = ceil_N >> lP;    // number of stages per thread
    const index_t di0        = ti * num_stages; // data batch index
    const index_t k0         = ti * num_stages; // stage index
    const index_t nux        = nu + nx;
    auto R̂ŜQ̂                 = riccati_R̂ŜQ̂.batch(ti);
    auto B̂                   = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
    auto Â                   = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
    auto BAᵀ                 = riccati_BAᵀ.batch(ti);
    auto A0                  = data_BA.batch(di0).right_cols(nx);
    // Copy B and A from the last stage
    {
        GUANAQO_TRACE("Riccati init", k0);
        copy(data_BA.batch(di0).left_cols(nu), B̂.left_cols(nu));
        syrk_diag_add(data_DCᵀ.batch(di0), tril(data_RSQ.batch(di0)), tril(R̂ŜQ̂.left_cols(nux)),
                      Σ.batch(di0));
    }
    for (index_t i = 0; i < num_stages; ++i) {
        index_t k = sub_wrap_N(k0, i);
        auto R̂ŜQ̂i = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto R̂Ŝi  = R̂ŜQ̂i.left_cols(nu);
        auto R̂i   = R̂Ŝi.top_rows(nu);
        auto Ŝi   = R̂Ŝi.bottom_rows(nx);
        auto Q̂i   = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i   = B̂.middle_cols(i * nu, nu);
        auto Âi   = Â.middle_cols(i * nx, nx);
        {
            GUANAQO_TRACE("Riccati QRS", k);
            using std::isfinite;
            if (isfinite(S))
                R̂i.add_to_diagonal(1 / S);
            // Factor R̂, update Ŝ, and compute LB̂ = B̂ LR̂⁻ᵀ
            potrf(tril(R̂Ŝi));                // ┐
            trsm(B̂i, tril(R̂i).transposed()); // ┘
            // Update Â = Ã - LB̂ LŜᵀ
            i == 0 ? gemm_sub(B̂i, Ŝi.transposed(), A0, Âi) //
                   : gemm_sub(B̂i, Ŝi.transposed(), Âi);
            if (isfinite(S))
                Q̂i.add_to_diagonal(1 / S);
            // Update and factor Q̂ = Q̃ - LŜ LŜᵀ
            syrk_sub_potrf(Ŝi, tril(Q̂i));
        }
        if (i + 1 < num_stages) {
            // Copy next B and A
            [[maybe_unused]] const auto k_next = sub_wrap_N(k, 1);
            GUANAQO_TRACE("Riccati update AB", k_next);
            const auto di_next = di0 + i + 1;
            auto BAᵀi          = BAᵀ.middle_cols(alt ? 0 : i * nx, nx);
            auto BAi           = data_BA.batch(di_next);
            auto Bi = BAi.left_cols(nu), Ai = BAi.right_cols(nx);
            // Compute next B̂ and Â
            auto B̂_next = B̂.middle_cols((i + 1) * nu, nu);
            auto Â_next = Â.middle_cols((i + 1) * nx, nx);
            gemm(Âi, Bi, B̂_next);
            gemm(Âi, Ai, Â_next);
            // Riccati update
            trmm(BAi.transposed(), tril(Q̂i), BAᵀi);
            auto R̂ŜQ̂_next = R̂ŜQ̂.middle_cols((i + 1) * nux, nux);
#if 1
            // TODO: merge with next potrf
            syrk_add(BAᵀi, tril(data_RSQ.batch(di_next)), tril(R̂ŜQ̂_next));
            syrk_diag_add(data_DCᵀ.batch(di_next), tril(R̂ŜQ̂_next), Σ.batch(di_next));
#else
            compact_blas::xsyrk_schur_copy(data_DCᵀ.batch(di_next), Σ.batch(di_next),
                                           data_RSQ.batch(di_next),
                                           R̂ŜQ̂_next);    // ┐
            compact_blas::xsyrk_add(BAᵀi, R̂ŜQ̂_next, be); // ┘
#endif
        } else {
            // Compute LÂ = Ã LQ⁻ᵀ
            GUANAQO_TRACE("Riccati last", k);
            trsm(Âi, tril(Q̂i).transposed());
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor(value_type S, view<> Σ, bool alt) {
    this->alt       = alt;
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [this, alt, S, Σ](index_t ti, index_t) {
        factor_riccati(ti, alt, S, Σ);
        factor_l0(ti);
        for (index_t l = 0; l < lP - lvl; ++l) {
            barrier();
            const index_t offset = 1 << l;
            const auto biY       = sub_wrap_PmV(ti, offset);
            const auto biU       = ti;
            if (is_active(l, biY))
                factor_schur_Y(l, biY);
            else if (is_active(l, biU))
                factor_schur_U(l, biU);
            else
                barrier();
        }
    });
}

} // namespace cyqlone
