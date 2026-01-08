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
    index_t c = ctx.index;
    //  2|  factor-block-column-riccati(c)    -- steps 1 and 2
    factor_riccati_solve<Factor, Solve>(ctx, S, Σ, ux, λ);
    //  3|  compute-schur(c)                  -- step 3
    compute_schur<Factor, Solve>(ctx, ux, λ);
    //  4|  factor-schur(c)                   -- step 4

    // 17|  for l = 0 ... log₂(P)-1
    for (index_t l = 0; l < lp(); ++l) { // Recursion level of cyclic reduction
        // 18|  iU = c+1, iY = c+1-2^l
        const auto iU = add_wrap_p(c, 1), iY = sub_wrap_p(c, (1 << l) - 1);
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
    if constexpr (Factor)
        if (solve_method == SolveMethod::PCR) {
            ctx.arrive_and_wait(); // wait for off-diagonal block
            if (ν2p(c + 1) + 1 == lp())
                factor_pcr();
        }
    if constexpr (Solve)
        if (ν2p(c + 1) + 1 == lp()) {
            if (solve_method == SolveMethod::PCR)
                solve_pcr(λ.batch(0), work_pcg.batch(0).left_cols(1));
            else
                solve_pcg(λ.batch(0), work_pcg.batch(0));
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
void CyqloneSolver<VL, T, DefaultOrder>::solve_forward_new(Context &ctx, mut_view<> ux,
                                                           mut_view<> λ) {
    factor_solve_impl<false, true>(ctx, 0, {}, ux, λ);
}

// Algorithm 5 “Solution of a symmetric block-tridiagonal system using cyclic reduction (CR)”
// §3.2 Cyclic reduction of block-tridiagonal linear systems
//
// The reverse solve routine below closely follows the structure of the corresponding factorization
// and forward solve routines, but in reverse order. An iterative approach is used instead of
// recursion. Note that the evaluation of λ(0) is performed during the forward solve step.

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_reverse_new(Context &ctx, mut_view<> ux,
                                                           mut_view<> λ, mut_view<> work) const {
    const index_t ti = ctx.index;
    for (index_t l = lp(); l-- > 0;) {
        const index_t i_u = add_wrap_p(ti, 1), i_y = sub_wrap_p(ti, (1 << l) - 1),
                      i_λ = l > 0 ? sub_wrap_p(ti, (1 << (l - 1)) - 1) : ti;
        ctx.arrive_and_wait(); // wait for λ
        if (ν2p(i_u) == l)
            solve_u_backward(l, i_u, λ, work);
        else if (ν2p(i_y) == l)
            solve_y_backward(l, i_y, λ);
        ctx.arrive_and_wait(); // wait for Uᵀλ, Yᵀλ
        if (ν2p(i_λ) == l)
            solve_λ_backward(i_λ, λ, work);
    }
    ctx.arrive_and_wait();
    solve_riccati_reverse_new(ctx, ux, λ, work);
}

} // namespace CYQLONE_NS(cyqlone)::v2
