#include <cyqlone/v2/cyqlone.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {

// Algorithm 2 “Cyqlone factorization”
// §4 “Cyqlone: Parallel factorization and solution of KKT systems with optimal control structure”
//
// Optionally fused factorization and forward solve of the KKT system.
//
// Differences compared to the pseudo-code in the paper:
//  - The penalty terms DCᵀ Σ DC and the regularizers Γₓ = SI are added to the cost Hessians during
//    the Riccati factorization step, as described in §5.1 “The augmented Lagrangian inner problem”.
//  - Solution is fused/interleaved with the factorization steps to improve temporal locality and
//    reduce memory bandwidth.
//  - The factorization and solution are done mostly in-place (without overwriting the OCP data).
//  - Factorization of the odd diagonal blocks M(i) is performed in the compute_schur function
//    instead of at the first level of the CR code.
//  - The last level is factored and solved using PCR or PCG, as described in §7.5.2 “Handling of
//    the final scalar levels”.

template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Factor, bool Solve>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder>::factor_solve_impl(Context &ctx, value_type S, view<> Σ,
                                                           mut_view<> ux, mut_view<> λ) {
    const index_t c = ctx.index;
    //  2|  factor-block-column-riccati(c)    -- steps 1 and 2
    factor_riccati_solve<Factor, Solve>(ctx, S, Σ, ux, λ);
    //  3|  compute-schur(c)                  -- step 3
    compute_schur<Factor, Solve>(ctx, ux, λ);
    //  4|  factor-schur(c)                   -- step 4

    // 17|  for l = 0 ... log₂(P)-1
    for (index_t l = 0; l < lp(); ++l) { // Recursion level of cyclic reduction
        const auto c_ = cr_thread_assignment(l, c);
        // 18|  iU = c+1, iY = c+1-2^l
        const auto iU = add_wrap_ceil_p(c_, 1), iY = sub_wrap_ceil_p(c_, (1 << l) - 1);
        // 19|  -- sync --
        ctx.arrive_and_wait(); // Wait for L
        // 20|  if ν₂(iU) = l:  U(iU) = K˂(iU) L(iU)⁻ᵀ
        if (ν2p(iU) == l) {
            if constexpr (Factor)
                factor_U(l, iU);
            if constexpr (Solve)
                solve_u_forward(l, iU, λ);
        }
        // 21|  elif ν₂(iY) = l:  Y(iY) = K˃(iY) L(iY)⁻ᵀ
        else if (ν2p(iY) == l) {
            if constexpr (Factor)
                factor_Y(l, iY);
            if constexpr (Solve)
                solve_y_forward(l, iY, λ, work_cr);
        }
        // 22|  -- sync --
        ctx.arrive_and_wait(); // Wait for U, Y
        // 23|  if ν₂(iU) = l:  factor-L(l, iY)
        if (ν2p(iU) == l) {
            if constexpr (Factor)
                factor_L(l, iY);
            if constexpr (Solve)
                solve_λ_forward(l, iY, λ, work_cr);
        }
        // 24|  elif ν₂(iY) = l:  update-K(l, iY)
        else if (ν2p(iY) == l) {
            if constexpr (Factor)
                update_K(l, iY);
        }
    }
    if constexpr (Factor) {
        if (solve_method == SolveMethod::PCR) {
            ctx.arrive_and_wait(); // wait for off-diagonal block
            if (ν2p(c + 1) + 1 == lp())
                factor_pcr();
        }
    }
    if constexpr (Solve) {
        if (solve_method == SolveMethod::PCR) {
            if constexpr (!Factor)
                ctx.arrive_and_wait(); // wait for off-diagonal block TODO: necessary?
            if (ν2p(c + 1) + 1 == lp())
                solve_pcr(λ.batch(0), work_pcg.batch(0).left_cols(1));
        } else {
            ctx.arrive_and_wait(); // wait for off-diagonal block
            if (ν2p(c + 1) + 1 == lp())
                solve_pcg(λ.batch(0), work_pcg.batch(0));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_solve(Context &ctx, value_type S, view<> Σ,
                                                      mut_view<> ux, mut_view<> λ) {
    factor_solve_impl<true, true>(ctx, S, Σ, ux, λ);
}
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor(Context &ctx, value_type S, view<> Σ) {
    factor_solve_impl<true, false>(ctx, S, Σ, {}, {});
}
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_forward(Context &ctx, mut_view<> ux, mut_view<> λ) {
    factor_solve_impl<false, true>(ctx, 0, {}, ux, λ);
}

// Algorithm 5 “Solution of a symmetric block-tridiagonal system using cyclic reduction (CR)”
// §3.2 Cyclic reduction of block-tridiagonal linear systems
//
// The reverse solve routine below closely follows the structure of the corresponding factorization
// and forward solve routines, but in reverse order. An iterative approach is used instead of
// recursion. Note that the evaluation of λ(0) is performed during the forward solve step.

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ,
                                                       mut_view<> work) const {
    for (index_t l = lp(); l-- > 0;) {
        ctx.arrive_and_wait(); // wait for λ
        {
            const auto c      = cr_thread_assignment(l, ctx.index);
            const index_t i_u = add_wrap_ceil_p(c, 1), i_y = sub_wrap_ceil_p(c, (1 << l) - 1);
            if (ν2p(i_u) == l)
                solve_u_backward(l, i_u, λ, work);
            else if (ν2p(i_y) == l)
                solve_y_backward(l, i_y, λ);
        }
        ctx.arrive_and_wait(); // wait for Uᵀλ, Yᵀλ
        {
            const auto c      = cr_thread_assignment(l - 1, ctx.index);
            const index_t i_λ = l > 0 ? sub_wrap_ceil_p(c, (1 << (l - 1)) - 1) : c;
            if (ν2p(i_λ) == l)
                solve_λ_backward(i_λ, λ, work);
        }
    }
    ctx.arrive_and_wait();
    solve_riccati_reverse(ctx, ux, λ, work);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ) {
    solve_reverse(ctx, ux, λ, riccati_work);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
index_t CyqloneSolver<VL, T, DefaultOrder>::cr_thread_assignment(index_t l, index_t c) const {
    // Adjust thread assignment for non-power-of-two p:
    // The diagonal blocks M(⌊p/2⌋2) are usually mapped to increasing thread indices as l increases.
    // (They curve to the right in the thread assignment diagram in the paper.)
    // However, these large thread indices are not actually present if p is not a power of two, so
    // we need to remap them. We always assign them to the even thread ⌊p/2⌋2, since this thread is
    // present even if p is odd. The odd thread ⌊p/2⌋2+1 is assigned the original index of the even
    // one, which is inactive.
    const auto ceil_p = 1 << lp();
    // c == p - 1 for odd p; c == p - 2 or c == p - 1 for even p
    const bool remapped_thread = (c >> 1) + 1 == (p + 1) >> 1;
    if (p < ceil_p && remapped_thread && l > 0)
        return (c & 1) ? (c ^ 1) : add_wrap_ceil_p(c, (1 << l) - 1);
    return c;
}

} // namespace CYQLONE_NS(cyqlone)::v2
