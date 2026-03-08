#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>
#include <cyqlone/tracing.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/trsm.hpp>

namespace CYQLONE_NS(cyqlone) {

using namespace linalg;
using namespace batmat::linalg;

// Algorithm 2 “Cyqlone factorization”
// §4.4 Factorization of the Schur complement (step 4)

//![Cyqlone factor CR helper]
// 20|  U(iU) = K˂(iU) L(iU)⁻ᵀ
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::factor_U([[maybe_unused]] index_t l, index_t iU) {
    if constexpr (v == 1)
        if (iU >= p && !circular) // happens in cases where p is not a power of two
            return;
    CYQ_TRACE_READ(Kb, iU, 0);
    CYQ_TRACE_READ(L, iU, 1);
    GUANAQO_TRACE("Trsm U", iU);
    CYQ_TRACE_WRITE(U, iU, 0);
    CYQ_TRACE_WRITE(U, iU, 1);
    trsm(cr_U.batch(iU), tril(cr_L.batch(iU)).transposed());
}

// 21|  Y(iY) = K˃(iY) L(iY)⁻ᵀ
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::factor_Y([[maybe_unused]] index_t l, index_t iY) {
    if constexpr (v == 1)
        if (iY + (1 << l) >= p && !circular) // Y(iY)=0 for scalar case
            return;
    CYQ_TRACE_READ(Kf, iY, 0);
    CYQ_TRACE_READ(L, iY, 0);
    GUANAQO_TRACE("Trsm Y", iY);
    CYQ_TRACE_WRITE(Y, iY, 0);
    CYQ_TRACE_WRITE(Y, iY, 1);
    trsm(cr_Y.batch(iY), tril(cr_L.batch(iY)).transposed());
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::update_K(index_t l, index_t i) {
    const index_t i_prev = sub_wrap_ceil_p(i, 1 << l), i_next = add_wrap_ceil_p(i, 1 << l);
    if constexpr (v == 1)
        if (i + (1 << l) >= p && !circular) // Y(i)=0 for scalar case
            return;
    CYQ_TRACE_READ(U, i, 1);
    CYQ_TRACE_READ(Y, i, 1);
    if (ν2p(i_prev) > ν2p(i_next)) {
        // 31|  K˂(i˃) = -U(i) Y(i)ᵀ
        GUANAQO_TRACE("Compute U", i_next);
        CYQ_TRACE_WRITE(Kb, i_next, 0);
        gemm_neg(cr_U.batch(i), cr_Y.batch(i).transposed(), cr_U.batch(i_next));
    } else {
        // 31|  K˃(i˂) = -Y(i) U(i)ᵀ
        GUANAQO_TRACE("Compute Y", i_prev);
        CYQ_TRACE_WRITE(Kf, i_prev, 0);
        gemm_neg(cr_Y.batch(i), cr_U.batch(i).transposed(), cr_Y.batch(i_prev));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::factor_L(index_t l, index_t i) {
    const index_t offset = 1 << l;
    const index_t iU     = add_wrap_ceil_p(i, offset);
    const index_t iY     = sub_wrap_ceil_p(i, offset);
    // Final block L(0) is stored separately (for PCR/PCG later)
    auto M = tril(cr_L.batch(i)), L0 = tril(pcr_L.batch(0));
    // 28|  if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
    const bool factor_next = ν2p(i) == l + 1;
    if constexpr (v == 1) {
        if (i == 0 && !circular) { // Y(iY)=0 for M on the first thread
            CYQ_TRACE_READ(M, i, 0);
            CYQ_TRACE_READ(U, iU, 0);
            GUANAQO_TRACE("Subtract UUᵀ", i);
            if (factor_next) {
                CYQ_TRACE_WRITE(L, i, 0);
                CYQ_TRACE_WRITE(L, i, 1);
            } else {
                CYQ_TRACE_WRITE(M, i, 0);
            }
            auto U = cr_U.batch(iU);
            // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
            // 28| if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
            factor_next ? syrk_sub_potrf(U, M, L0) // chol(M - UUᵀ)
                        : syrk_sub(U, M);
            return;
        } else if (iU >= p && !circular) { // happens in cases where p is not a power of two
            CYQ_TRACE_READ(M, i, 0);
            CYQ_TRACE_READ(Y, iY, 0);
            GUANAQO_TRACE("Subtract YYᵀ", i);
            if (factor_next) {
                CYQ_TRACE_WRITE(L, i, 0);
                CYQ_TRACE_WRITE(L, i, 1);
            } else {
                CYQ_TRACE_WRITE(M, i, 0);
            }
            auto Y = cr_Y.batch(iY);
            // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
            // 28| if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
            factor_next ? syrk_sub_potrf(Y, M) // chol(M - YYᵀ)
                        : syrk_sub(Y, M);
            return;
        }
    }
    auto U = cr_U.batch(iU), Y = cr_Y.batch(iY);
    {
        CYQ_TRACE_READ(M, i, 0);
        CYQ_TRACE_READ(U, iU, 0);
        GUANAQO_TRACE("Subtract UUᵀ", i);
        CYQ_TRACE_WRITE(M, i, 0);
        // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
        syrk_sub(U, M);
    }
    if (factor_next && i != 0) {
        CYQ_TRACE_READ(M, i, 0);
        CYQ_TRACE_READ(Y, iY, 0);
        GUANAQO_TRACE("Factor M", i);
        CYQ_TRACE_WRITE(L, i, 0);
        CYQ_TRACE_WRITE(L, i, 1);
        // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
        // 28|  if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
        syrk_sub_potrf(Y, M); // chol(M - YYᵀ)
    } else {
        CYQ_TRACE_READ(M, i, 0);
        CYQ_TRACE_READ(Y, iY, 0);
        GUANAQO_TRACE("Subtract YYᵀ", i);
        CYQ_TRACE_WRITE(M, i, 0);
        // 27|  M(i)⁺ = M(i) - U(iU) U(iU)ᵀ - Y(iY) Y(iY)ᵀ
        if (i != 0)
            syrk_sub(Y, M);
        else if constexpr (v > 1)
            syrk_sub(Y, M, with_rotate_C<1>, with_rotate_D<1>);
        else if (circular)
            syrk_sub(Y, M);
    }
    // 28| if ν₂(i) = l+1:  L(i) = chol(M(i)⁺)
    if (factor_next && i == 0) {
        CYQ_TRACE_READ(M, i, 0);
        GUANAQO_TRACE("Factor M", i);
        CYQ_TRACE_WRITE(L, i, 0);
        CYQ_TRACE_WRITE(L, i, 1);
        potrf(M, L0);
    }
}
//![Cyqlone factor CR helper]

// Algorithm 5 “CR: Solution of a symmetric block-tridiagonal system using cyclic reduction”
// §3.2 Cyclic reduction of block-tridiagonal linear systems
//
// The solve routines below closely follow the structure of the corresponding factorization routines.

//![Cyqlone solve CR helper]
template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::solve_u_forward(index_t l, index_t iU, mut_view<> λ,
                                                               index_t stride) const {
    if constexpr (v == 1)
        if (iU >= p && !circular) // happens in cases where p is not a power of two
            return;
    const index_t iL  = sub_wrap_ceil_p(iU, 1 << l); // = k, iU = k+2^l
    const index_t diU = iU * stride, diL = iL * stride;
    // 16|  b(0)⁺ = b(0) - U(2^l) b̃(2^l)
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    GUANAQO_TRACE("Subtract Ub", iL);
    gemv_sub(cr_U.batch(iU), λ.batch(diU), λ.batch(diL));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::solve_y_forward(index_t l, index_t iY, mut_view<> λ,
                                                               mut_view<> w, index_t stride) const {
    if constexpr (v == 1)
        if (iY + (1 << l) >= p && !circular) // Y(iY)=0 for scalar case
            return;
    const index_t iL  = add_wrap_ceil_p(iY, 1 << l); // = k, iY = k-2^l
    const index_t diY = iY * stride;
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    GUANAQO_TRACE("Subtract Yb", iL);
    gemv(cr_Y.batch(iY), λ.batch(diY), w.batch(iL));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::solve_λ_forward(index_t l, index_t iL, mut_view<> λ,
                                                               view<> w, index_t stride) const {
    const index_t diL = iL * stride;
    const index_t iY  = sub_wrap_ceil_p(iL, 1 << l);
    // 21|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    if (v > 1 || iY + (1 << l) < p || circular) { // Equilvalent to iL >= (1 << l), kept for clarity
        // b(diL) -= w(iL)
        GUANAQO_TRACE("Subtract work b", iL);
        iL == 0 ? sub(λ.batch(diL), w.batch(iL), with_rotate<-1>) //
                : sub(λ.batch(diL), w.batch(iL));
    }
    // 14|  b̃(k)⁺ = L(k)⁻¹ b(k)⁺    -- for the next level
    if (ν2p(iL) == l + 1 && iL != 0) { // Don't solve the last level here
        GUANAQO_TRACE("Solve b", iL);
        // solve L(diL)⁻¹ b(diL)
        trsm(tril(cr_L.batch(iL)), λ.batch(diL));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::solve_u_backward(index_t l, index_t iU, mut_view<> λ,
                                                                mut_view<> w,
                                                                index_t stride) const {
    if constexpr (v == 1)
        if (iU >= p && !circular) // happens in cases where p is not a power of two
            return;
    const index_t iL  = sub_wrap_ceil_p(iU, 1 << l); // = k, iU = k+2^l
    const index_t diL = iL * stride;
    // 25|  x(k) = L(k)⁻ᵀ (b̃(k) - Y(k)ᵀ x(k+2^l) - U(k)ᵀ x(k-2^l))
    GUANAQO_TRACE("Subtract Uᵀb", iL);
    // w[iU] = U[iU]ᵀ λ[diL]
    gemv(cr_U.batch(iU).transposed(), λ.batch(diL), w.batch(iU));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::solve_y_backward(index_t l, index_t iY, mut_view<> λ,
                                                                index_t stride) const {
    if constexpr (v == 1)
        if (iY + (1 << l) >= p && !circular) // Y(iY)=0 for scalar case
            return;
    const index_t iL  = add_wrap_ceil_p(iY, 1 << l); // = k, iY = k-2^l
    const index_t diL = iL * stride, diY = iY * stride;
    auto Y = cr_Y.batch(iY);
    // 25|  x(k) = L(k)⁻ᵀ (b̃(k) - Y(k)ᵀ x(k+2^l) - U(k)ᵀ x(k-2^l))
    GUANAQO_TRACE("Subtract Yᵀb", iL);
    // b[diY] -= Y[iY]ᵀ b[diL]
    v == 1 || iL > 0 ? gemv_sub(Y.transposed(), λ.batch(diL), λ.batch(diY)) //
                     : gemv_sub(Y.transposed(), λ.batch(diL), λ.batch(diY), with_rotate_B<1>);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::solve_λ_backward(index_t iL, mut_view<> λ, view<> w,
                                                                index_t stride) const {
    const index_t diL = iL * stride; // iL = k
    // 25|  x(k) = L(k)⁻ᵀ (b̃(k) - Y(k)ᵀ x(k+2^l) - U(k)ᵀ x(k-2^l))
    { // λ[diL] -= w[iL]
        GUANAQO_TRACE("Subtract work b", iL);
        sub(λ.batch(diL), w.batch(iL));
    }
    // solve D⁻ᵀ[diL] d[diL]
    GUANAQO_TRACE("Solve b", iL);
    BATMAT_ASSUME(iL != 0);
    trsm(tril(cr_L.batch(iL)).transposed(), λ.batch(diL));
}
//![Cyqlone solve CR helper]

// Prefetching

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
template <StorageOrder O>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::prefetch(batch_view<O> X) const {
    if (!params.enable_prefetching)
        return;
    const auto inner_stride = std::max<index_t>(64 / sizeof(value_type) / v, 1);
    if constexpr (O == StorageOrder::RowMajor)
        for (index_t r = 0; r < X.rows(); ++r)
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t c = 0; c < X.cols(); c += inner_stride)
                __builtin_prefetch(&X(0, r, c), 0, 2);
    else
        for (index_t c = 0; c < X.cols(); ++c)
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = 0; r < X.rows(); r += inner_stride)
                __builtin_prefetch(&X(0, r, c), 0, 2);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
template <StorageOrder O>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::prefetch_L(batch_view<O> X) const {
    if (!params.enable_prefetching)
        return;
    const auto inner_stride = std::max<index_t>(64 / sizeof(value_type) / v, 1);
    if constexpr (O == StorageOrder::RowMajor)
        for (index_t r = 0; r < X.rows(); ++r)
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t c = 0; c <= r; c += inner_stride)
                __builtin_prefetch(&X(0, r, c), 0, 2);
    else
        for (index_t c = 0; c < X.cols(); ++c)
            BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = c; r < X.rows(); r += inner_stride)
                __builtin_prefetch(&X(0, r, c), 0, 2);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::prefetch_L(index_t bi) const {
    GUANAQO_TRACE("prefetch L", bi);
    prefetch_L(cr_L.batch(bi));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::prefetch_U([[maybe_unused]] index_t l,
                                                          index_t iU) const {
    if (v == 1 && iU >= p)
        return;
    GUANAQO_TRACE("prefetch U", iU);
    prefetch(cr_U.batch(iU));
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void TricyqleSolver<VL, T, DefaultOrder, Ctx>::prefetch_Y(index_t l, index_t iY) const {
    if (v == 1 && iY + (1 << l) >= p && !circular)
        return;
    GUANAQO_TRACE("prefetch Y", iY);
    prefetch(cr_Y.batch(iY));
}

} // namespace CYQLONE_NS(cyqlone)
