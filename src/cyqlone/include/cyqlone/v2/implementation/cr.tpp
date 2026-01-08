#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/trsm.hpp>

#ifndef CYQLONE_FACTOR_DO_PREFETCH
#define CYQLONE_FACTOR_DO_PREFETCH 0
#endif

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

// Algorithm 2 “Cyqlone factorization”
// §4.4 Factorization of the Schur complement (step 4)
//
// Cyclic reduction helper functions.
//
// Differences compared to the pseudo-code in the paper:
//  - The factorization is done in-place on cr_L, cr_U, and cr_Y. Subdiagonal
//    blocks K˂ and K˃ are temporarily stored in cr_U and cr_Y respectively.
//  - Syrk and potrf operations are fused where possible to improve performance.

// 20|  U(iU) = K˂(iU) L(iU)⁻ᵀ
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_U([[maybe_unused]] index_t l, index_t iU) {
    GUANAQO_TRACE("Trsm U", iU);
    trsm(cr_U.batch(iU), tril(cr_L.batch(iU)).transposed());
}

// 21|  Y(iY) = K˃(iY) L(iY)⁻ᵀ
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_Y([[maybe_unused]] index_t l, index_t iY) {
    GUANAQO_TRACE("Trsm Y", iY);
    trsm(cr_Y.batch(iY), tril(cr_L.batch(iY)).transposed());
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_K(index_t l, index_t i) {
    const index_t i_prev = sub_wrap_p(i, 1 << l), i_next = add_wrap_p(i, 1 << l);
#if CYQLONE_FACTOR_DO_PREFETCH
    for (index_t c = 0; c < cr_U.cols(); c += 1)
        for (index_t r = 0; r < cr_U.rows(); r += 16)
            __builtin_prefetch(&cr_U.batch(i)(0, r, c), 0, 3);
#endif
    if (ν2p(i_prev) > ν2p(i_next)) {
        // 31|  K˂(i˃) = -U(i) Y(i)ᵀ
        GUANAQO_TRACE("Compute U", i_next);
        gemm_neg(cr_U.batch(i), cr_Y.batch(i).transposed(), cr_U.batch(i_next));
    } else {
        // 31|  K˃(i˂) = -Y(i) U(i)ᵀ
        GUANAQO_TRACE("Compute Y", i_prev);
        gemm_neg(cr_Y.batch(i), cr_U.batch(i).transposed(), cr_Y.batch(i_prev));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_L(index_t l, index_t i) {
    const index_t offset = 1 << l;
    const index_t iU     = add_wrap_p(i, offset);
    const index_t iY     = sub_wrap_p(i, offset);
    auto M               = tril(cr_L.batch(i));
    auto U = cr_U.batch(iU), Y = cr_Y.batch(iY);
#if CYQLONE_FACTOR_DO_PREFETCH
    for (index_t c = 0; c < cr_Y.cols(); c += 1)
        for (index_t r = 0; r < cr_Y.rows(); r += 16)
            __builtin_prefetch(&cr_Y.batch(iY)(0, r, c), 0, 3);
#endif
    {
        GUANAQO_TRACE("Subtract UUᵀ", i);
        // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
        syrk_sub(U, M);
    }
    if (ν2p(i) == l + 1 && i != 0) {
        GUANAQO_TRACE("Factor M", i);
        // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
        // 28|  if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
        syrk_sub_potrf(Y, M); // chol(M - YYᵀ)
    } else {
        GUANAQO_TRACE("Subtract YYᵀ", i);
        // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
        i == 0 ? syrk_sub(Y, M, with_rotate_C<1>, with_rotate_D<1>, with_mask_D<1>)
               : syrk_sub(Y, M);
    }
    // 28| if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
    if (ν2p(i) == l + 1 && i == 0) {
        GUANAQO_TRACE("Factor M", i);
        potrf(M, tril(pcr_L.batch(0))); // Final block is stored separately (for PCR/PCG later)
    }
}

// Algorithm 5 “Solution of a symmetric block-tridiagonal system using cyclic reduction (CR)”
// §3.2 Cyclic reduction of block-tridiagonal linear systems
//
// The solve routines below closely follow the structure of the corresponding factorization routines.
//
// Differences compared to the pseudo-code in the paper:
//  - We use an iterative approach to factor all levels, instead of recursion.
//  - The right-hand side vector λ is updated in-place.
//  - It contains all stages of the original problem, not just the stages that are handled by CR.
//    Therefore, we use the data batch index di = n bi, not the cyclic reduction batch index bi.
//  - Y(k-2^l) b̃(k-2^l) is stored in a temporary workspace to allow it to be evaluated concurrently
//    with U(k+2^l) b̃(k+2^l), as they both update b(k)⁺. Similarly for the backward solve, where
//    U(k)ᵀ x(k-2^l) is stored in a temporary workspace to avoid races on x(k).
//  - The last level is not handled here, because it is solved using PCG or PCR.

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_fwd_level(index_t l, index_t iU,
                                                         mut_view<> λ) const {
    const index_t n   = ceil_N >> lP;
    const index_t r   = 1 << l;
    const index_t iL  = sub_wrap_p(iU, r); // k
    const index_t iY  = sub_wrap_p(iL, r); // k-2^l
    const index_t diU = iU * n, diL = iL * n, diY = iY * n;
    auto Y = cr_Y.batch(iY);
    // 16|  b(0)⁺ = b(0) - U(2^l) b̃(2^l)
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    { // b(diL) -= U(iU) b(diU)
        GUANAQO_TRACE("Subtract Ub", iL);
        gemv_sub(cr_U.batch(iU), λ.batch(diU), λ.batch(diL));
    }
    { // b(diL) -= Y(iY) b(diY)
        GUANAQO_TRACE("Subtract Yb", iL);
        iL == 0 ? gemv_sub(Y, λ.batch(diY), λ.batch(diL), //
                           with_rotate_C<1>, with_rotate_D<1>, with_mask_D<1>)
                : gemv_sub(Y, λ.batch(diY), λ.batch(diL));
    }
    // 14|  b̃(k)⁺ = L(k)⁻¹ b(k)⁺    -- for the next level
    if (ν2p(iL) == l + 1) {
        GUANAQO_TRACE("Solve b", iL);
        BATMAT_ASSUME(iL != 0);
        // solve L(diL)⁻¹ b(diL)
        trsm(tril(cr_L.batch(iL)), λ.batch(diL));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_u_forward(index_t l, index_t iU,
                                                         mut_view<> λ) const {
    const index_t n   = ceil_N >> lP;
    const index_t r   = 1 << l;
    const index_t iL  = sub_wrap_p(iU, r); // = k, iU = k+2^l
    const index_t diU = iU * n, diL = iL * n;
    // 16|  b(0)⁺ = b(0) - U(2^l) b̃(2^l)
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    GUANAQO_TRACE("Subtract Ub", iL);
    gemv_sub(cr_U.batch(iU), λ.batch(diU), λ.batch(diL));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_y_forward(index_t l, index_t iY, mut_view<> λ,
                                                         mut_view<> w) const {
    const index_t n   = ceil_N >> lP;
    const index_t r   = 1 << l;
    const index_t iL  = add_wrap_p(iY, r); // = k, iY = k-2^l
    const index_t diY = iY * n;
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    GUANAQO_TRACE("Subtract Yb", iL);
    gemv(cr_Y.batch(iY), λ.batch(diY), w.batch(iL));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_λ_forward(index_t l, index_t iL, mut_view<> λ,
                                                         view<> w) const {
    const index_t n   = ceil_N >> lP;
    const index_t diL = iL * n;
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    { // b(diL) -= w(iL)
        GUANAQO_TRACE("Subtract work b", iL);
        iL == 0 ? compact_blas::template xsub<1>(simdify(λ.batch(diL)), simdify(w.batch(iL)))
                : compact_blas::template xsub<0>(simdify(λ.batch(diL)), simdify(w.batch(iL)));
    }
    // 14|  b̃(k)⁺ = L(k)⁻¹ b(k)⁺    -- for the next level
    if (ν2p(iL) == l + 1) {
        GUANAQO_TRACE("Solve b", iL);
        BATMAT_ASSUME(iL != 0);
        // solve L(diL)⁻¹ b(diL)
        trsm(tril(cr_L.batch(iL)), λ.batch(diL));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_u_backward(index_t l, index_t iU, mut_view<> λ,
                                                          mut_view<> w) const {
    const index_t n   = ceil_N >> lP;
    const index_t r   = 1 << l;
    const index_t iL  = sub_wrap_p(iU, r); // = k, iU = k+2^l
    const index_t diL = iL * n;
    // 25|  x(k) = L(k)⁻ᵀ (b̃(k) - Y(k)ᵀ x(k+2^l) - U(k)ᵀ x(k-2^l))
    GUANAQO_TRACE("Subtract Uᵀb", iL);
    // w[iU] = U[iU]ᵀ λ[diL]
    gemv(cr_U.batch(iU).transposed(), λ.batch(diL), w.batch(iU));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_y_backward(index_t l, index_t iY,
                                                          mut_view<> λ) const {
    const index_t n   = ceil_N >> lP;
    const index_t r   = 1 << l;
    const index_t iL  = add_wrap_p(iY, r); // = k, iY = k-2^l
    const index_t diL = iL * n, diY = iY * n;
    auto Y = cr_Y.batch(iY);
    // 25|  x(k) = L(k)⁻ᵀ (b̃(k) - Y(k)ᵀ x(k+2^l) - U(k)ᵀ x(k-2^l))
    GUANAQO_TRACE("Subtract Yᵀb", iL);
    // b[diY] -= Y[iY]ᵀ b[diL]
    iL == 0 ? gemv_sub(Y.transposed(), λ.batch(diL), λ.batch(diY), with_shift_B<1>)
            : gemv_sub(Y.transposed(), λ.batch(diL), λ.batch(diY));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_λ_backward(index_t iL, mut_view<> λ,
                                                          view<> w) const {
    const index_t n   = ceil_N >> lP;
    const index_t diL = iL * n; // iL = k
    // 25|  x(k) = L(k)⁻ᵀ (b̃(k) - Y(k)ᵀ x(k+2^l) - U(k)ᵀ x(k-2^l))
    { // λ[diL] -= w[iL]
        GUANAQO_TRACE("Subtract work b", iL);
        compact_blas::xsub(simdify(λ.batch(diL)), simdify(w.batch(iL)));
    }
    // solve D⁻ᵀ[diL] d[diL]
    GUANAQO_TRACE("Solve b", iL);
    BATMAT_ASSUME(iL != 0);
    trsm(tril(cr_L.batch(iL)).transposed(), λ.batch(diL));
}

} // namespace CYQLONE_NS(cyqlone)::v2
