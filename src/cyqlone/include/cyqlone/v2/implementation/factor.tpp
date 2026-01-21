#include <cyqlone/v2/cyqlone.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {

// Algorithm 2 “Cyqlone factorization”
// §4 “Cyqlone: Parallel factorization and solution of KKT systems with optimal control structure”
//
// Optionally fused factorization and forward solve of the KKT system.
//
// Differences compared to the pseudo-code in the paper:
//  - The penalty terms DCᵀ Σ DC and the regularizers Γₓ = γI are added to the cost Hessians during
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
void CyqloneSolver<VL, T, DefaultOrder>::factor_solve_impl(Context &ctx, value_type γ, view<> Σ,
                                                           mut_view<> ux, mut_view<> λ) {
    const index_t c = ctx.index;
    //  2|  factor-block-column-riccati(c)    -- steps 1 and 2
    factor_riccati_solve<Factor, Solve>(ctx, γ, Σ, ux, λ);
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
    // Factor or solve the last level using PCR or PCG
    if constexpr (Factor) {
        if (solve_method == SolveMethod::PCR) {
            ctx.arrive_and_wait(); // wait for off-diagonal block
            if (ν2p(c + 1) + 1 == lp() || p == 1)
                factor_pcr();
        }
    }
    if constexpr (Solve) {
        if (solve_method == SolveMethod::PCR) {
            if constexpr (!Factor)
                ctx.arrive_and_wait(); // wait for off-diagonal block TODO: necessary?
            if (ν2p(c + 1) + 1 == lp() || p == 1)
                solve_pcr(λ.batch(0), work_pcg.batch(0).left_cols(1));
        } else {
            ctx.arrive_and_wait(); // wait for off-diagonal block
            if (ν2p(c + 1) + 1 == lp() || p == 1)
                solve_pcg(λ.batch(0), work_pcg.batch(0));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor_solve(Context &ctx, value_type γ, view<> Σ,
                                                      mut_view<> ux, mut_view<> λ) {
    factor_solve_impl<true, true>(ctx, γ, Σ, ux, λ);
}
template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::factor(Context &ctx, value_type γ, view<> Σ) {
    factor_solve_impl<true, false>(ctx, γ, Σ, {}, {});
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
    const index_t c = ctx.index;
    for (index_t l = lp(); l-- > 0;) {
        const auto c_     = cr_thread_assignment(l, c);
        const index_t i_u = add_wrap_ceil_p(c_, 1), i_y = sub_wrap_ceil_p(c_, (1 << l) - 1);
        if (l < lp() - 1) {              // λ(0) was already computed during forward solve
            auto wait_uy = ctx.arrive(); // wait for Uᵀλ, Yᵀλ
            if (ν2p(i_y) == l + 1) {
                ctx.wait(std::move(wait_uy));
                solve_λ_backward(i_y, λ, work);
            } else if (ν2p(i_u) == l) {
                prefetch_U(l, i_u);
                ctx.wait(std::move(wait_uy));
            } else {
                if (ν2p(i_y) == l)
                    prefetch_Y(l, i_y);
                ctx.wait(std::move(wait_uy));
            }
        }
        auto wait_λ = ctx.arrive(); // wait for λ
        if (ν2p(i_u) == l) {
            ctx.wait(std::move(wait_λ));
            solve_u_backward(l, i_u, λ, work);
        } else if (ν2p(i_y) == l) {
            ctx.wait(std::move(wait_λ));
            solve_y_backward(l, i_y, λ);
        } else {
            if (l > 0) {
                const auto l_next = l - 1, c_next = cr_thread_assignment(l_next, c);
                const index_t i_u_next = add_wrap_ceil_p(c_next, 1),
                              i_y_next = sub_wrap_ceil_p(c_next, (1 << l_next) - 1);
                if (ν2p(i_y_next) == l_next + 1) {
                    prefetch_U(l_next, i_u_next);
                    prefetch_L(i_y_next);
                }
            }
            ctx.wait(std::move(wait_λ));
        }
    }
    ctx.arrive_and_wait(); // wait for Uᵀλ, Yᵀλ
    if (ν2p(c) == 0 && p != 1)
        solve_λ_backward(c, λ, work);
    ctx.arrive_and_wait(); // wait for λ(c-1)
    solve_riccati_reverse(ctx, ux, λ, work);
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ) {
    solve_reverse(ctx, ux, λ, riccati_work);
}

/// Adjust thread assignment for non-power-of-two p:
/// The diagonal blocks M(⌊p/2⌋2) are usually mapped to increasing thread indices c as the CR level
/// l increases, as can be seen in the functions above, where iY = c + 1 - 2^l, and from the way the
/// path of M nodes curves to the right in the thread assignment diagram in the paper.
/// However, these large thread indices are not actually present if p is not a power of two, so
/// we need to remap them, undoing the offset 1 - 2^l.
/// We always assign the last M evaluation to the even thread ⌊p/2⌋2, since this thread is present
/// even if p is odd. The odd thread ⌊p/2⌋2+1 is assigned an inactive index, since it never has any
/// work during CR, as there is no coupling between the last and first stages (at least not in the
/// scalar case).
template <index_t VL, class T, StorageOrder DefaultOrder>
index_t CyqloneSolver<VL, T, DefaultOrder>::cr_thread_assignment(index_t l, index_t c) const {
    // Index of the last diagonal block M or L that may need to be handled in this level
    const auto iL = c & ~index_t{(1 << l) - 1};
    // Only remap the last two threads: c == p - 1 for odd p; c == p - 2 or c == p - 1 for even p
    const bool last_threads = (c >> 1) + 1 == (p + 1) >> 1;
    // If this block iL would be assigned to a thread >= p, remap it to the last even thread < p
    const bool remap = iL + (1 << l) - 1 >= p;
    if (!is_pow_2(p) && last_threads && remap)
        c = c & 1 ? iL                                 // last odd thread gets the inactive index
                  : add_wrap_ceil_p(iL, (1 << l) - 1); // last even thread gets remapped
    return c;
}

} // namespace CYQLONE_NS(cyqlone)::v2
