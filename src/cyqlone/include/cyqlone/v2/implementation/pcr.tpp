#include <cyqlone/v2/cyqlone.hpp>

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

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

// Algorithm 6 “Solution of a symmetric block-tridiagonal system using parallel cyclic
//              reduction (PCR)”
//
// Differences compared to the pseudo-code in the paper:
//  - The factorization is done in-place on pcr_L.
//  - The solution is done in-place on the input λ.
//  - We use an iterative approach to factor all levels, instead of recursion.
//  - The solution step is separated from the factorization step.

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_pcr() {
    [this]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template factor_pcr_level<Levels>(), ...);
    }(std::make_integer_sequence<index_t, CyqloneSolver::lvl>{});
}

// The level is a template parameter to allow for compile-time vector rotations.
// The number of levels is small, so this should not bloat the code too much.
template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void CyqloneSolver<VL, T, DefaultOrder>::factor_pcr_level() {
    GUANAQO_TRACE("Factor PCR", Level);
    auto M      = Level == 0 ? cr_L.batch(0) : pcr_M.batch(0);
    auto K      = Level == 0 ? cr_Y.batch(0) : pcr_Y.batch(Level);
    auto M_next = pcr_M.batch(0);
    auto L = pcr_L.batch(Level), Y = pcr_Y.batch(Level), U = pcr_U.batch(Level);
    static constexpr auto r = 1 << Level; // 2^l
    //  8|  U(k) = K(k-2^l)ᵀ L(k)⁻ᵀ
    trsm(K.transposed(), triu(L.transposed()), U, with_rotate_A<-r>);
    //  7|  Y(k) = K(k) L(k)⁻ᵀ
    trsm(K, triu(L.transposed()), Y);
    // 10|  M(k)⁺ = M(k) - Y(k-2^l) Y(k-2^l)ᵀ - U(k+2^l) U(k+2^l)ᵀ
    //      -- implemented as M(k-2^l)⁺ = M(k-2^l) - Y(k) Y(k)ᵀ
    syrk_sub(U, tril(M), tril(M_next), with_rotate_C<-r>, with_rotate_D<-r>);
    //      -- followed by    M(k+2^l)⁺ -= U(k) U(k)ᵀ
    syrk_sub(Y, tril(M_next), with_rotate_C<+r>, with_rotate_D<+r>);
    //  3|  L(k)⁺ = chol(M(k)⁺)    -- for the next level
    potrf(tril(M_next), tril(pcr_L.batch(Level + 1)));
    if constexpr (Level + 1 < lvl) {
        auto K_next = pcr_Y.batch(Level + 1);
        // 11|  K(k)⁺ = -Y(k+2^l) U(k+2^l)ᵀ    -- implemented as K(k-2^l)⁺ = -Y(k) U(k)ᵀ
        gemm_neg(Y, U.transposed(), K_next, {}, with_rotate_C<-r>, with_rotate_D<-r>);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_pcr(mut_batch_view<> λ,
                                                   mut_batch_view<> work_pcr) const {
    [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template solve_pcr_level<Levels>(λ, work_pcr), ...);
    }(std::make_integer_sequence<index_t, lvl>{});
    GUANAQO_TRACE("Solve PCR", lvl);
    //  5|  x(k) = L(k)⁻ᵀ L(k)⁻¹ b(k)
    trsm(tril(pcr_L.batch(lvl)), λ);
    trsm(triu(pcr_L.batch(lvl).transposed()), λ);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void CyqloneSolver<VL, T, DefaultOrder>::solve_pcr_level(mut_batch_view<> λ,
                                                         mut_batch_view<> work_pcr) const {
    GUANAQO_TRACE("Solve PCR", Level);
    auto L = pcr_L.batch(Level), Y = pcr_Y.batch(Level), U = pcr_U.batch(Level);
    static constexpr auto r = 1 << Level;
    //  9|  b̃(k) = L(k)⁻¹ b(k)
    trsm(tril(L), λ, work_pcr); // w = L⁻¹ λ
    // 12|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    gemv_sub(Y, work_pcr, λ, with_rotate_C<+r>, with_rotate_D<+r>, with_mask_D<+r>);
    gemv_sub(U, work_pcr, λ, with_rotate_C<-r>, with_rotate_D<-r>, with_mask_D<-r>);
}

} // namespace CYQLONE_NS(cyqlone)::v2
