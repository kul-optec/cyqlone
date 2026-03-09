# Algorithms {#algorithms}

This page lists the implementations of the algorithms described in the
Cyqlone paper @cite pas_cyqlone_2025,
with discussions of the differences compared to the pseudo-code,
and with line-by-line comments referencing the corresponding steps in the paper.

## Algorithm 1: Factorization of a single modified Riccati block column {#algorithm-1}

Factorization of the smaller OCPs on each sub-interval using a modified Riccati recursion.
Optionally fused with the forward solve.

Differences compared to the pseudo-code in the paper:
 - Many operations are performed in-place to reduce memory usage.
   For example, the matrices R̂, Ŝ and Q̂ are replaced by their Cholesky factors LR, LS and LQ.
 - Solution is fused/interleaved with the factorization steps to improve temporal locality and
   reduce memory bandwidth. This is controlled by the `Factor` and `Solve` template parameters.
 - The addition of the penalty term DCᵀ Σ DC is fused with the rest of the operations,
   avoiding an explicit formation of the intermediate matrix and improving cache locality.
   See §5.1 “The augmented Lagrangian inner problem” for details about the penalty term.
 - The product V(j-1) V(j-1)ᵀ is not added to the Hessian at the end of an iteration, but rather
   at the beginning of the next iteration, so it can be fused with the addition of DCᵀ Σ DC
   and the Cholesky factorization of the sum.
 - Data batch indices where the problem data and the factorization are stored are reversed
   compared to the stage indices, matching the iteration order and simplifying the per-thread
   contiguous storage.

@snippet{lineno} cyqlone/include/cyqlone/implementation/riccati.tpp Modified Riccati factorization and fused forward solve

## Algorithm 2: Cyqlone factorization {#algorithm-2}

Factorization of the entire KKT system using the Cyqlone algorithm. Optionally fused with the
forward solve.

Described by §4 “Cyqlone: Parallel factorization and solution of KKT systems with optimal control structure”

Differences compared to the pseudo-code in the paper:
 - The penalty terms DCᵀ Σ DC and the regularizers Γₓ = γI are added to the cost Hessians during
   the Riccati factorization step, as described in §5.1 “The augmented Lagrangian inner problem”.
 - Solution is fused/interleaved with the factorization steps to improve temporal locality and
   reduce memory bandwidth.
 - The factorization and solution are done mostly in-place (without overwriting the OCP data).
 - Factorization of the odd diagonal blocks M(i) is performed in the compute_schur function
   instead of at the first level of the CR code.
 - The last level is factored and solved using PCR or PCG, as described in §7.5.2 “Handling of
   the final scalar levels”.

### High-level factorization procedure

@snippet{lineno} cyqlone/include/cyqlone/implementation/factor.tpp Cyqlone factorization and fused forward solve

### Schur complement computation

This is the <span style="font-variant:small-caps">compute-schur</span> function in the paper,
including the factorization of the first level of CR.

@snippet{lineno} cyqlone/include/cyqlone/implementation/schur.tpp Cyqlone compute Schur

### Schur complement factorization

This is the <span style="font-variant:small-caps">factor-schur</span> function in the paper, but
without the first level of CR, which is fused with the
<span style="font-variant:small-caps">compute-schur</span> function above.

@snippet{lineno} cyqlone/include/cyqlone/implementation/factor.tpp Cyqlone factor Schur

### CR helper functions

Differences compared to the pseudo-code in the paper:
 - The factorization is done in-place on cr_L, cr_U, and cr_Y. Subdiagonal blocks K˂ and K˃ are
   temporarily stored in cr_U and cr_Y respectively.
 - Syrk and potrf operations are fused where possible to improve performance.
 - Additional masking is performed for the scalar case (v == 1), corresponding to the boundary
   conditions K˃(p-2^l)=0 (i.e. no periodic coupling between the last and first stages). This
   serves two main purposes: it avoids unnecessary computations on zero blocks, and it allows
   for processor counts p that are not powers of two. In contrast, the vectorized case requires
   periodic boundary conditions, so this masking is not applied for v > 1.

@snippet{lineno} cyqlone/include/cyqlone/implementation/cr.tpp Cyqlone factor CR helper

## Algorithm 3: Factorization update of a single modified Riccati block column {#algorithm-3}

Differences compared to the pseudo-code in the paper:
 - Many operations are performed in-place to reduce memory usage.
   For example, all original Cholesky factors are replaced by the updated ones.
 - Solution is fused/interleaved with the factorization steps to improve temporal locality and
   reduce memory bandwidth.
 - The workspaces Υ1 and Υ2 are reused for the variables Υ and Φ in the paper. Two workspaces
   are required because the matrix multiplication by Φx(j) cannot be done in-place.
 - Only the constraints for which ΔΣ is nonzero are used during the update. This is done by
   compressing the relevant columns of Dᵀ and Cᵀ into Υu and Υx respectively.
 - A global communication step is used at the end to compute the total update rank for the entire
   problem, and to partition the workspace for Υ˃ and Υ˂ to prepare for the CR phase.
 - The update for u(0) is handled as a special case to exploit its mostly independent structure.
 - If the number of processors p is not a power of two, the workspace allocation of Υ˃(0) is
   adjusted to ensure that it does not overlap with Υ˂(p-2^l). Note that this is only necessary
   when u(0) is not isolated. See @ref cyqlone::TricyqleSolver::work_Ups_fwd_w "work_Ups_fwd_w".
 - In the vectorized case, Υ˃(0) and Υ˂(0) are stored in different workspaces in the last level
   of CR, since this is not actually the last level of the full reduction (PCR handles the rest).
   See @ref cyqlone::TricyqleSolver::work_Ups_bwd_w "work_Ups_bwd_w".

@snippet{lineno} cyqlone/include/cyqlone/implementation/update.tpp Cyqlone update Riccati

## Algorithm 4: Cyqlone factorization updates {#algorithm-4}

Differences compared to the pseudo-code in the paper:
 - The update of the last has been modified to allow for vectorization (v>1), updating the
   PCR factorization if necessary.
 - Solution is fused/interleaved with the factorization steps to improve temporal locality and
   reduce memory bandwidth.
 - A heuristic rank check is used to decide whether to update or re-factorize the last level.
 - The update matrices Y˃(0) are skipped when they are zero (i.e. when the updates to u(0) are
   handled separately). This saves some unnecessary computation in the scalar case.

### High-level update procedure

@snippet{lineno} cyqlone/include/cyqlone/implementation/update.tpp Cyqlone update

### Update of the CR factorization

@snippet{lineno} cyqlone/include/cyqlone/implementation/update.tpp Cyqlone update CR

### CR factorization update helper functions

Most of the space here is taken up by the updates of the last level, which needs to handle some
special cases depending on the final PCR or PCG solver, and depending on whether we perform
updates or re-factorization.

The special cases `if constexpr (v == 1)` add some visual overhead, and can safely be ignored.

@snippet{lineno} cyqlone/include/cyqlone/implementation/update.tpp Cyqlone update CR helper

## Algorithm 5: CR: Solution of a symmetric block-tridiagonal system using cyclic reduction {#algorithm-5}

Differences compared to the pseudo-code in the paper:
 - We use an iterative approach to factor all levels, instead of recursion.
 - The right-hand side vector λ is updated in-place.
 - It contains all stages of the original problem, not just the stages that are handled by CR.
   Therefore, we use the data batch index di = n bi, not the cyclic reduction batch index bi.
 - Y(k-2^l) b̃(k-2^l) is stored in a temporary workspace to allow it to be evaluated concurrently
   with U(k+2^l) b̃(k+2^l), as they both update b(k)⁺. Similarly for the backward solve, where
   U(k)ᵀ x(k-2^l) is stored in a temporary workspace to avoid races on x(k).
 - The last level is not handled here, because it is solved using PCG or PCR.
 - The forward solve is fused with the factorization above.

### Serial reverse solve

@snippet{lineno} cyqlone/include/cyqlone/implementation/factor.tpp Cyqlone solve CR serial

### Parallel reverse solve

@snippet{lineno} cyqlone/include/cyqlone/implementation/factor.tpp Cyqlone solve CR

### CR solve helper functions

@snippet{lineno} cyqlone/include/cyqlone/implementation/cr.tpp Cyqlone solve CR helper

## Algorithm 6: PCR: Solution of a symmetric block-tridiagonal system using parallel cyclic reduction {#algorithm-6}

Differences compared to the pseudo-code in the paper:
 - The solution step is separated from the factorization step. For the factorization, we use the
   periodic version [below](#algorithm-7).
 - The solution is done in-place on the input λ.
 - We use an iterative approach to factor all levels, instead of recursion.

@snippet{lineno} cyqlone/include/cyqlone/implementation/pcr.tpp Cyqlone solve PCR

## Algorithm 7: Periodic PCR factorization of a block-tridiagonal matrix {#algorithm-7}

Differences compared to the pseudo-code in the paper:
 - The factorization is done in-place on pcr_L, and the intermediate matrices K˂ and K˃ are stored
   in pcr_U and pcr_Y, before solving them in-place.
 - Triangular solves of the subdiagonal blocks are optionally parallelized.

### Serial PCR factorization

@snippet{lineno} cyqlone/include/cyqlone/implementation/pcr.tpp PCR factor serial

### Parallel PCR factorization

@snippet{lineno} cyqlone/include/cyqlone/implementation/pcr.tpp PCR factor

## Algorithm 8: Periodic PCR factorization updates by a block-bidiagonal matrix {#algorithm-8}

Differences compared to the pseudo-code in the paper:
 - Updates are performed in-place on pcr_L, pcr_U and pcr_Y.
 - Intermediate update matrices are left rotated in memory to minimize the number of rotations
   required.

@snippet{lineno} cyqlone/include/cyqlone/implementation/update.tpp PCR update
