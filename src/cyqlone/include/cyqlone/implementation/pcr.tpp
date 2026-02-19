#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>

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
#include <batmat/ops/rotate.hpp>
#include <batmat/simd.hpp>
#include <utility>

namespace CYQLONE_NS(cyqlone) {

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
void TricyqleSolver<VL, T, DefaultOrder>::factor_pcr() {
    [this]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template factor_pcr_level<Levels>(), ...);
    }(std::make_integer_sequence<index_t, lv()>{});
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void TricyqleSolver<VL, T, DefaultOrder>::factor_pcr_parallel(Context &ctx) {
    [this, &ctx]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template factor_pcr_level_parallel<Levels>(ctx), ...);
    }(std::make_integer_sequence<index_t, lv()>{});
}

// The level is a template parameter to allow for compile-time vector rotations.
// The number of levels is small, so this should not bloat the code too much.
template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void TricyqleSolver<VL, T, DefaultOrder>::factor_pcr_level() {
    GUANAQO_TRACE("Factor PCR", Level);
    auto M      = Level == 0 ? cr_L.batch(0) : pcr_M.batch(0);
    auto K      = Level == 0 ? cr_Y.batch(0) : pcr_Y.batch(Level);
    auto M_next = pcr_M.batch(0);
    auto L = pcr_L.batch(Level), Y = pcr_Y.batch(Level), U = pcr_U.batch(Level);
    static constexpr auto r = 1 << Level; // 2^l

    if constexpr (Level + 1 == lv() && merge_last_level_pcr) {
        // In the last level, we only have a single sub-diagonal block, which is computed as
        // K(k) = -Y(k+2^l) U(k+2^l)ᵀ - U(k-2^l) Y(k-2^l)ᵀ. Since 2^l = -2^l mod v, we only need to
        // compute one term, and then add its transpose, K(k) ← K(k) + K(k+2^l)ᵀ. Because the right
        // half of the batches in K are zero in the absence of coupling between the first and
        // last blocks, we can perform the transposition in-place.
        if (!circular) {
            GUANAQO_TRACE("Merge last PCR level", Level, K.depth() / 2 * K.rows() * K.cols());
            using namespace batmat::datapar;
            using simd_half = deduced_simd<T, v / 2>;
            for (index_t j = 0; j < K.cols(); ++j)
                for (index_t i = 0; i < K.rows(); ++i)
                    aligned_store(aligned_load<simd_half>(&K(0, j, i)), &K(v / 2, i, j));
        } else {
            GUANAQO_TRACE("Merge last PCR level", Level, 2 * K.depth() * K.rows() * K.cols());
            // In case of circular coupling, we cannot exploit the complementarity of the batches,
            // so we cannot perform the transposition in-place. Instead, we transpose it into U
            // first (U is not used here, so we can overwrite it), and then add it to K.
            // TODO: is there a better way?
            batmat::linalg::copy(K.transposed(), U, with_rotate<-r>);
            linalg::add(K, U);
        }
    }

    //  8|  U(k) = K(k-2^l)ᵀ L(k)⁻ᵀ
    trsm(K.transposed(), triu(L.transposed()), U, with_rotate_A<-r>);
    //  7|  Y(k) = K(k) L(k)⁻ᵀ
    if constexpr (Level + 1 < lv() || !merge_last_level_pcr)
        trsm(K, triu(L.transposed()), Y);
    // 10|  M(k)⁺ = M(k) - Y(k-2^l) Y(k-2^l)ᵀ - U(k+2^l) U(k+2^l)ᵀ
    //      -- implemented as M(k-2^l)⁺ = M(k-2^l) - Y(k) Y(k)ᵀ
    syrk_sub(U, tril(M), tril(M_next), with_rotate_C<-r>, with_rotate_D<-r>);
    //      -- followed by    M(k+2^l)⁺ -= U(k) U(k)ᵀ
    if constexpr (Level + 1 < lv() || !merge_last_level_pcr)
        syrk_sub(Y, tril(M_next), with_rotate_C<+r>, with_rotate_D<+r>);
    //  3|  L(k)⁺ = chol(M(k)⁺)    -- for the next level
    potrf(tril(M_next), tril(pcr_L.batch(Level + 1)));
    if constexpr (Level + 1 < lv()) {
        auto K_next = pcr_Y.batch(Level + 1);
        // 11|  K(k)⁺ = -Y(k+2^l) U(k+2^l)ᵀ    -- implemented as K(k-2^l)⁺ = -Y(k) U(k)ᵀ
        gemm_neg(Y, U.transposed(), K_next, {}, with_rotate_C<-r>, with_rotate_D<-r>);
        // TODO: we could store K_next in U instead of Y, so the last level would not need extra
        //       storage. But this is more complex, as we need to transpose it here, so we can
        //       perform the trsm in the next level in-place (which is not possible if the input
        //       and output are transposed).
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void TricyqleSolver<VL, T, DefaultOrder>::factor_pcr_level_parallel(Context &ctx) {
    auto M      = Level == 0 ? cr_L.batch(0) : pcr_M.batch(0);
    auto K      = Level == 0 ? cr_Y.batch(0) : pcr_L.batch(Level + 1);
    auto M_next = pcr_M.batch(0);
    auto L = pcr_L.batch(Level), Y = pcr_Y.batch(Level), U = pcr_U.batch(Level);
    static constexpr auto r = 1 << Level; // 2^l

    // Use the same thread assignment as CR
    BATMAT_ASSUME(ctx.num_thr >= 2);
    const bool primary   = ν2p(ctx.index + 1) + 1 == lp(),
               secondary = ν2p(ctx.index + 1 + p / 2) + 1 == lp();

    if (secondary && Level + 1 == lv()) {
        GUANAQO_TRACE("Merge last PCR level", Level, K.depth() / 2 * K.rows() * K.cols());
        // In the last level, we only have a single sub-diagonal block, which is computed as
        // K(k) = -Y(k+2^l) U(k+2^l)ᵀ - U(k-2^l) Y(k-2^l)ᵀ. Since 2^l = -2^l mod v, we only need to
        // compute one term, and then add its transpose, K(k) ← K(k) + K(k+2^l)ᵀ. Because the right
        // half of the batches in K are zero in the absence of coupling between the first and
        // last blocks, we can perform the transposition in-place.
        if (!circular) {
            using namespace batmat::datapar;
            using simd_half = deduced_simd<T, v / 2>;
            for (index_t j = 0; j < K.cols(); ++j)
                for (index_t i = 0; i < K.rows(); ++i)
                    aligned_store(aligned_load<simd_half>(&K(0, j, i)), &K(v / 2, i, j));
        } else {
            GUANAQO_TRACE("Merge last PCR level", Level, 2 * K.depth() * K.rows() * K.cols());
            // In case of circular coupling, we cannot exploit the complementarity of the batches,
            // so we cannot perform the transposition in-place. Instead, we transpose it into U
            // first (U is not used here, so we can overwrite it), and then add it to K.
            // TODO: is there a better way?
            batmat::linalg::copy(K.transposed(), U, with_rotate<-r>);
            linalg::add(K, U);
        }
    }

    ctx.arrive_and_wait(); // wait for L and K

    if (primary) {
        GUANAQO_TRACE("Factor PCR U", Level);
        //  8|  U(k) = K(k-2^l)ᵀ L(k)⁻ᵀ
        trsm(K.transposed(), triu(L.transposed()), U, with_rotate_A<-r>);
    } else if (secondary && Level + 1 < lv()) {
        GUANAQO_TRACE("Factor PCR Y", Level);
        //  7|  Y(k) = K(k) L(k)⁻ᵀ
        trsm(K, triu(L.transposed()), Y);
    }

    if (Level + 1 < lv())
        ctx.arrive_and_wait(); // wait for U and Y

    if (primary) {
        GUANAQO_TRACE("Factor PCR L", Level);
        // 10|  M(k)⁺ = M(k) - Y(k-2^l) Y(k-2^l)ᵀ - U(k+2^l) U(k+2^l)ᵀ
        //      -- implemented as M(k-2^l)⁺ = M(k-2^l) - Y(k) Y(k)ᵀ
        syrk_sub(U, tril(M), tril(M_next), with_rotate_C<-r>, with_rotate_D<-r>);
        //      -- followed by    M(k+2^l)⁺ -= U(k) U(k)ᵀ
        if constexpr (Level + 1 < lv() || !merge_last_level_pcr)
            syrk_sub(Y, tril(M_next), with_rotate_C<+r>, with_rotate_D<+r>);
        //  3|  L(k)⁺ = chol(M(k)⁺)    -- for the next level
        potrf(tril(M_next), tril(pcr_L.batch(Level + 1)));
    } else if (secondary && Level + 1 < lv()) {
        GUANAQO_TRACE("Factor PCR K", Level);
        auto K_next = pcr_L.batch(Level + 2);
        // 11|  K(k)⁺ = -Y(k+2^l) U(k+2^l)ᵀ    -- implemented as K(k-2^l)⁺ = -Y(k) U(k)ᵀ
        gemm_neg(Y, U.transposed(), K_next, {}, with_rotate_C<-r>, with_rotate_D<-r>);
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void TricyqleSolver<VL, T, DefaultOrder>::solve_pcr(mut_batch_view<> λ,
                                                    mut_batch_view<> work_pcr) const {
    [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
        (this->template solve_pcr_level<Levels>(λ, work_pcr), ...);
    }(std::make_integer_sequence<index_t, lv()>{});
    GUANAQO_TRACE("Solve PCR", lv());
    //  5|  x(k) = L(k)⁻ᵀ L(k)⁻¹ b(k)
    trsm(tril(pcr_L.batch(lv())), λ);
    trsm(triu(pcr_L.batch(lv()).transposed()), λ);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
template <index_t Level>
void TricyqleSolver<VL, T, DefaultOrder>::solve_pcr_level(mut_batch_view<> λ,
                                                          mut_batch_view<> work_pcr) const {
    GUANAQO_TRACE("Solve PCR", Level);
    auto L = pcr_L.batch(Level), Y = pcr_Y.batch(Level), U = pcr_U.batch(Level);
    static constexpr auto r = 1 << Level;
    //  9|  b̃(k) = L(k)⁻¹ b(k)
    trsm(tril(L), λ, work_pcr); // w = L⁻¹ λ
    // 12|  b(k)⁺ = b(k) - Y(k-2^l) b̃(k-2^l) - U(k+2^l) b̃(k+2^l)
    if constexpr (Level + 1 < lv() || !merge_last_level_pcr)
        gemv_sub(Y, work_pcr, λ, with_rotate_C<+r>, with_rotate_D<+r>);
    gemv_sub(U, work_pcr, λ, with_rotate_C<-r>, with_rotate_D<-r>);
}

} // namespace CYQLONE_NS(cyqlone)
