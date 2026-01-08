#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/trsm.hpp>
#include <batmat/linalg/trtri.hpp>
#include <utility>

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

// Algorithm 2 “Cyqlone factorization”
// §4.3 “Computation of the Schur complement (step 3)”
//
// Build the Schur complement after factorizing the Riccati blocks, and/or update the right-hand
// side of the Schur complement after performing a forward solve of the Riccati blocks.
//
// See also: factor.tpp

template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Factor, bool Solve>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder>::compute_schur(Context &ctx, mut_view<> ux, mut_view<> λ) {
    const index_t c   = ctx.index;
    const auto c_next = add_wrap_p(c, 1);
    //  7|  j₁ = n(c-1)+1,  jₙ = nc
    const auto dn = c * n, dn_next = c_next * n, d1_next = dn_next + n - 1;
    //  8|  i˃ = c,  i˂ = c-1
    const index_t i_fwd = c, i_bwd = sub_wrap_p(c, 1);
    auto M = tril(cr_L.batch(c));
    // 13|  W = [ LB(jₙ) ... LB(j₁) LA(j₁) ]    -- The order here is [ LA(j₁) LB(jₙ) ... LB(j₁) ]
    auto W = riccati_ÂB̂.batch(c).right_cols(nx + nu * n);
    if constexpr (Factor) {
        auto R̂ŜQ̂ = riccati_R̂ŜQ̂.batch(c);
        auto LQ  = tril(R̂ŜQ̂.bottom_right(nx, nx));
        //  9|  T(c) = LQ(j₁)⁻ᵀ
        BATMAT_ASSERT(nu >= 1); // T = LQ⁻ᵀ is upper triangular, stored one row up from LQ itself
        auto Tc = triu(R̂ŜQ̂.right_cols(nx).middle_rows(nu - 1, nx));
        {
            GUANAQO_TRACE("Invert Q", c);
            trtri(LQ, Tc.transposed());
        }
        auto T_ready = ctx.arrive();
        auto LA1     = riccati_ÂB̂.batch(c).middle_cols(nx * (n - 1), nx); // LA(j₁)
        // 10|  if ν2(i˂) > ν2(i˃)    K˂(i˃) = -T(c) LA(j₁)ᵀ    else    K˃(i˂) = -LA(j₁) T(c)ᵀ
        if (ν2p(i_bwd) > ν2p(i_fwd)) {
            GUANAQO_TRACE("Compute first U", i_fwd);
            trmm_neg(Tc, LA1.transposed(), cr_U.batch(i_fwd));
        } else {
            GUANAQO_TRACE("Compute first Y", i_bwd);
            i_fwd == 0 ? trmm_neg(LA1, Tc.transposed(), cr_Y.batch(i_bwd), //
                                  with_rotate_C<-1>, with_rotate_D<-1>, with_mask_D<-1>)
                       : trmm_neg(LA1, Tc.transposed(), cr_Y.batch(i_bwd));
        }
        // 11|  -- sync --
        //      Wait for the inversion in the next interval
        ctx.wait(std::move(T_ready));
        //      Each column of the cyclic part with coupling equations is updated by two threads:
        //      one for the forward, and one for the backward coupling. Update the diagonal blocks
        //      of the coupling equations, first forward in time ...
        auto R̂ŜQ̂_next = riccati_R̂ŜQ̂.batch(c_next);
        // 12|  M(c)˂ = T(c+1) T(c+1)ᵀ
        auto Tc_next = triu(R̂ŜQ̂_next.right_cols(nx).middle_rows(nu - 1, nx));
        {
            GUANAQO_TRACE("Compute TTᵀ", c_next);
            c_next == 0 ? trmm(Tc_next, Tc_next.transposed(), M, //
                               with_rotate_C<-1>, with_rotate_D<-1>, with_mask_D<-1>)
                        : trmm(Tc_next, Tc_next.transposed(), M);
        }
        //      And finally backward in time, optionally fused with the factorization.
        if (lP == lvl) { // no multi-threading
            GUANAQO_TRACE("Factor M last", c);
            // 14|  M(c) = M(c)˂ + M(c)˃ = M(c)˂ + WWᵀ
            syrk_add(W, M);
            // 16|  L(c) = chol(M(c))
            potrf(M, tril(pcr_L.batch(0))); // Final block is stored separately (for PCR/PCG later)
        } else if (ν2p(i_fwd) == 0) {
            GUANAQO_TRACE("Factor M", c);
            // 14|  M(c) = M(c)˂ + M(c)˃ = M(c)˂ + WWᵀ
            // 16|  L(c) = chol(M(c))
            syrk_add_potrf(W, M);
        } else {
            GUANAQO_TRACE("Compute WWᵀ", c);
            // 14|  M(c) = M(c)˂ + M(c)˃ = M(c)˂ + WWᵀ
            syrk_add(W, M);
        }
    }
    if constexpr (Solve) {
        if (!Factor)
            ctx.arrive_and_wait(); // Wait for x_next
        {
            GUANAQO_TRACE("Update λ", dn);
            auto x_next = ux.batch(d1_next).bottom_rows(nx);
            c_next == 0 ? compact_blas::template xsub<-1>(simdify(λ.batch(dn)), simdify(x_next))
                        : compact_blas::template xsub<+0>(simdify(λ.batch(dn)), simdify(x_next));
        }
        {
            GUANAQO_TRACE("Solve λ", dn);
            if (ν2p(i_fwd) == 0)
                trsm(M, λ.batch(dn));
        }
    }
}

} // namespace CYQLONE_NS(cyqlone)::v2
