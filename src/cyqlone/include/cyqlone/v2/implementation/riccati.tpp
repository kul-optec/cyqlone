#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/linalg/compress.hpp>
#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/trsm.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

// Algorithm 1 “Factorization of a single modified Riccati block column”
//
// Differences compared to the pseudo-code in the paper:
//  - Many operations are performed in-place to reduce memory usage.
//    For example, the matrices R̂, Ŝ and Q̂ are replaced by the Cholesky factors LR, LS and LQ.
//  - The addition of the penalty term DCᵀ Σ DC is fused with the rest of the operations,
//    avoiding an explicit formation of the intermediate matrix and improving cache locality.
//    See §5.1 “The augmented Lagrangian inner problem” for details about the penalty term.
//  - The product V(j-1) V(j-1)ᵀ is not added to the Hessian at the end of an iteration, but rather
//    at the beginning of the next iteration, so it can be fused with the addition of DCᵀ Σ DC
//    and the Cholesky factorization of the sum.
//  - Data batch indices where the problem data and the factorization are stored are reversed
//    compared to the stage indices, simplifying the per-thread contiguous storage.

template <index_t VL, class T, StorageOrder DefaultOrder>
template <bool Factor, bool Solve>
// NOLINTNEXTLINE(*-cognitive-complexity) // Needs to match pseudocode structure
void CyqloneSolver<VL, T, DefaultOrder>::factor_riccati_solve(Context &ctx, value_type S, view<> Σ,
                                                              mut_view<> ux, mut_view<> λ) {
    constexpr bool no_keep_V = true; // Don't store intermediate V = BAᵀ LQ products
    using batmat::linalg::compress_masks_sqrt;
    using std::isfinite;
    const index_t c = ctx.index;
    //  3|  j₁ = n(c-1)+1, jₙ = nc
    const index_t dn  = c * n; // data batch index
    const index_t jn  = c * n; // stage index
    const index_t nux = nu + nx;
    const index_t nyM = std::max(ny, ny_0 + ny_N); // max active constraints/stage
    // TODO: special case nyM for c == 0
    auto R̂ŜQ̂ = riccati_R̂ŜQ̂.batch(c);
    auto B̂ = riccati_ÂB̂.batch(c).right_cols(n * nu), Â = riccati_ÂB̂.batch(c).left_cols(n * nx);
    auto VDCᵀ      = riccati_BAᵀ.batch(c);
    index_t m_syrk = 0; // number of columns of VDCᵀ (depends on active constraints)
    if constexpr (Factor) {
        GUANAQO_TRACE("Riccati init", jn);
        //  4|  B̂(jₙ) = B(jₙ)
        // Note that Â(jₙ) is not copied explicitly, as it is not modified in-place
        copy(data_BA.batch(dn).left_cols(nu), B̂.left_cols(nu));
        // Compress the active constraint Jacobians to add them to the Hessian later
        m_syrk = compress_masks_sqrt(data_DCᵀ.batch(dn), Σ.batch(dn), VDCᵀ.left_cols(nyM));
    }
    // Iterate over all stages in the interval (in reverse order)
    for (index_t i = 0; i < n; ++i) {
        //  6|  for j = jₙ downto j₁
        const index_t j  = sub_wrap_N(jn, i); // stage index j ≡ jₙ - i mod N
        const index_t di = dn + i;            // data batch index
        auto R̂ŜQ̂i        = R̂ŜQ̂.middle_cols(i * nux, nux);
        auto R̂Ŝi         = R̂ŜQ̂i.left_cols(nu);
        auto R̂i = R̂Ŝi.top_rows(nu), Ŝi = R̂Ŝi.bottom_rows(nx), Q̂i = R̂ŜQ̂i.bottom_right(nx, nx);
        auto B̂i = B̂.middle_cols(i * nu, nu), Âi = Â.middle_cols(i * nx, nx);
        {
            GUANAQO_TRACE("Riccati QRS", j);
            // Compute and factor R̂, update Ŝ, factor Q̂
            //
            // 13|  [ R̂(j)  Ŝ(j) ] = [ R(j)  S(j) ] + [ D(j) ]ᵀ Σ(j) [ D(j)  C(j) ] + V(j) V(j)ᵀ
            //   |  [ Ŝ(j)ᵀ Q̂(j) ]   [ S(j)ᵀ Q(j) ]   [ C(j) ]
            //
            //  7|  [ LR(j)       ] = chol [ R̂(j)  Ŝ(j) ]
            //   |  [ LS(j) LQ(j) ]        [ Ŝ(j)ᵀ Q̂(j) ]
            if constexpr (Factor) {
                auto VDCᵀ_prev = VDCᵀ.middle_cols(no_keep_V || i == 0 ? 0 : (i - 1) * nx, m_syrk);
                syrk_add_potrf(VDCᵀ_prev, tril(data_RSQ.batch(di)), tril(R̂ŜQ̂i), 1 / S);
            }
            if constexpr (Solve) {
                // Solve u ← LR̂⁻¹ u, x ← x - Ŝ u
                auto ui = ux.batch(di).top_rows(nu), xi = ux.batch(di).bottom_rows(nx);
                trsm(tril(R̂i), ui);
                gemv_sub(Ŝi, ui, xi);
            }
            //  8|  LB(j) = B̂(j) LR(j)⁻ᵀ
            if constexpr (Factor) {
                trsm(B̂i, tril(R̂i).transposed());
            }
            if constexpr (Solve) {
                auto ui = ux.batch(di).top_rows(nu), λ_last = λ.batch(dn);
                gemv_add(B̂i, ui, λ_last);
            }
            //  9|  Acl(j) = Â(j) - LB(j) LS(j)ᵀ
            if constexpr (Factor) {
                //  4|  Â(jₙ) = A(jₙ)
                auto An = data_BA.batch(dn).right_cols(nx);
                i == 0 ? gemm_sub(B̂i, Ŝi.transposed(), An, Âi) //
                       : gemm_sub(B̂i, Ŝi.transposed(), Âi);
            }
        }
        // 10|  if j > j₁
        if (i + 1 < n) {
            [[maybe_unused]] const auto j_next = sub_wrap_N(j, 1);
            GUANAQO_TRACE("Riccati update AB", j_next);
            const auto di_next = dn + i + 1;
            auto VDCᵀ_next     = VDCᵀ.middle_cols(no_keep_V ? 0 : i * nx, nx + nyM);
            auto BAᵀ_next = VDCᵀ_next.left_cols(nx), DCᵀ_next = VDCᵀ_next.right_cols(nyM);
            auto BAi = data_BA.batch(di_next);
            auto Bi = BAi.left_cols(nu), Ai = BAi.right_cols(nx);
            // 11|  [ B̂(j-1)  Â(j-1) ] = Acl(j) [ B(j-1)  A(j-1) ]
            if constexpr (Factor) {
                auto B̂_next = B̂.middle_cols((i + 1) * nu, nu),
                     Â_next = Â.middle_cols((i + 1) * nx, nx);
                gemm(Âi, Bi, B̂_next);
                gemm(Âi, Ai, Â_next);
            }
            if constexpr (Solve) {
                auto xi = ux.batch(di).bottom_rows(nx), ux_next = ux.batch(di_next),
                     λ_next = λ.batch(di_next), λ_last = λ.batch(dn);
                gemv_add(Âi, λ_next, λ_last); // λ(jn) += Â λ(j-1)
                auto w = work_cr.batch(c).left_cols(1);
                trmm(tril(Q̂i).transposed(), λ_next, w); // w = LQᵀ(j) λ(j-1)
                trmm(tril(Q̂i), w);                      // w = LQ(j) LQᵀ(j) λ(j-1)
                compact_blas::xsub_copy(simdify(w), simdify(xi),
                                        simdify(w));    // w = x(j) - LQ(j) LQᵀ(j) λ(j-1)
                gemv_add(BAi.transposed(), w, ux_next); // u(j-1) += BAᵀ(j-1) w
            }
            // 12|  V(j-1) = [ B(j-1)ᵀ ] LQ(j)
            //   |           [ A(j-1)ᵀ ]
            if constexpr (Factor) {
                trmm(BAi.transposed(), tril(Q̂i), BAᵀ_next);
                m_syrk = nx; // columns of V(j-1)
                // Compress the active constraint Jacobians to add them to the Hessian later
                m_syrk += compress_masks_sqrt(data_DCᵀ.batch(di_next), Σ.batch(di_next), DCᵀ_next);
            }
        } else {
            GUANAQO_TRACE("Riccati last", j);
            // 14|  LA(j₁) = Â(j₁) LQ(j₁)⁻ᵀ
            if constexpr (Factor) {
                trsm(Âi, tril(Q̂i).transposed());
            }
            if constexpr (Solve) {
                auto xi = ux.batch(di).bottom_rows(nx), λ_last = λ.batch(dn);
                trsm(tril(Q̂i), xi);
                gemv_add(Âi, xi, λ_last);
                trsm(tril(Q̂i).transposed(), xi);
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_riccati_reverse(Context &ctx, mut_view<> ux,
                                                               mut_view<> λ,
                                                               mut_view<> work) const {
    const index_t c       = ctx.index;
    const index_t c_prev  = sub_wrap_p(c, 1);
    const index_t jn      = c * n;      // stage index
    const index_t dn      = c * n;      // jₙ data batch index
    const index_t dn_prev = c_prev * n; // j₀ data batch index
    const index_t nux     = nu + nx;
    const auto L          = riccati_R̂ŜQ̂.batch(c);
    const auto LB         = riccati_ÂB̂.batch(c).right_cols(n * nu);
    const auto AclLA      = riccati_ÂB̂.batch(c).left_cols(n * nx);
    const auto λn         = λ.batch(dn);
    const auto w          = work.batch(c);

    for (index_t i = n; i-- > 0;) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        index_t di                 = dn + i;
        const auto Li = L.middle_cols(i * nux, nux), LQi = Li.bottom_right(nx, nx),
                   LRi = Li.top_left(nu, nu), LSi = Li.bottom_left(nx, nu);
        const auto LBi = LB.middle_cols(i * nu, nu);
        if (i + 1 < n) {
            const auto di_prev = di + 1;
            GUANAQO_TRACE("Riccati solve rev", j);
            const auto ui = ux.batch(di).top_rows(nu), xi = ux.batch(di).bottom_rows(nx);
            const auto Acl     = AclLA.middle_cols(i * nx, nx);
            const auto BA_prev = data_BA.batch(di_prev);
            const auto λ_prev  = λ.batch(di_prev);

            // w = q(j)
            copy(xi, w);
            // x(j) = A(j-1) x(j-1) + B u(j-1) + b(j-1)
            gemv_add(BA_prev, ux.batch(di_prev), λ_prev, xi);
            // u(j) = LR(j)⁻ᵀ(r(j) - LS(j)ᵀ x(j) - LB(j)ᵀ λ(jₙ))
            gemv_sub(LBi.transposed(), λn, ui);
            gemv_sub(LSi.transposed(), xi, ui);
            trsm(tril(LRi).transposed(), ui);

            // λ(j-1) = LQ(j) LQ(j)ᵀ x(j) + Aclᵀ λ(jₙ) - q(j)
            trmm(tril(LQi).transposed(), xi, λ_prev);
            trmm(tril(LQi), λ_prev);
            gemv_add(Acl.transposed(), λn, λ_prev);
            compact_blas::xsub(simdify(λ_prev), simdify(w));
        } else {
            GUANAQO_TRACE("Riccati solve rev", j);
            const auto u1 = ux.batch(di).top_rows(nu), x1 = ux.batch(di).bottom_rows(nx);
            const auto LA1 = AclLA.middle_cols(i * nx, nx);
#if 1
            // w = LQ(j₁)⁻¹ λ(j₀)
            c == 0 ? trsm(tril(LQi), λ.batch(dn_prev), w, with_rotate_B<-1>)
                   : trsm(tril(LQi), λ.batch(dn_prev), w);
#else
            const bool x_lanes = c == 0;
            x_lanes ? compact_blas::template xadd_copy<1>(simdify(w), simdify(λ.batch(dn_prev)))
                    : compact_blas::template xadd_copy<0>(simdify(w), simdify(λ.batch(dn_prev)));
            // LQ⁻¹ λ
            trsm(tril(LQi), w);
#endif
            // w = LQ(j₁)⁻¹ λ(j₀) - LA(j₁)ᵀ λ(jₙ)
            gemv_sub(LA1.transposed(), λn, w);
            // w = LQ(j₁)⁻ᵀ(LQ(j₁)⁻¹ λ(j₀) - LA(j₁)ᵀ λ(jₙ))
            trsm(tril(LQi).transposed(), w);
            // x(j₁) = LQ(j₁)⁻ᵀ(LQ(j₁)⁻¹ λ(j₀) - LA(j₁)ᵀ λ(jₙ)) + q(j₁)
            compact_blas::xadd(simdify(x1), simdify(w));

            // u(j₁) = LR(j₁)⁻ᵀ(r(j₁) - LB(j₁)ᵀ λ(jₙ) - LS(j₁)ᵀ x(j₁))
            gemv_sub(LBi.transposed(), λn, u1);
            gemv_sub(LSi.transposed(), x1, u1);
            trsm(tril(LRi).transposed(), u1);
        }
    }
}

} // namespace CYQLONE_NS(cyqlone)::v2
