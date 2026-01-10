#include <cyqlone/v2/cyqlone.hpp>

#include <print>

#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/syomv.hpp>
#include <batmat/linalg/trsm.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

// §7.5.2 “Handling of the final scalar levels”
//
// Straightforward preconditioned conjugate gradient method to solve M(v) λ = b, with optimized
// vectorized matrix-vector products for M(v) and its preconditioner.

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::mul_Mv(batch_view<> p, mut_batch_view<> Mp,
                                                batch_view<default_order> L,
                                                batch_view<default_order> K) const -> value_type {
    // Mp = M p = LLᵀ p + K p
    trmm(triu(L.transposed()), p, Mp);
    trmm(tril(L), Mp);
    syomv(tril(K), p, Mp);
    return compact_blas::xdot(simdify(p), simdify(Mp));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::mul_precond(batch_view<> r, mut_batch_view<> z,
                                                     mut_batch_view<> w,
                                                     batch_view<default_order> L,
                                                     batch_view<default_order> K) const
    -> value_type {
    // Stair: z = Φ⁻¹ r = L⁻ᵀL⁻¹ r - L⁻ᵀL⁻¹ K L⁻ᵀL⁻¹ r = L⁻ᵀL⁻¹(r - K L⁻ᵀL⁻¹ r)
    copy(r, z);
    if (solve_method == SolveMethod::StairPCG) {
        trsm(tril(L), r, w);
        trsm(triu(L.transposed()), w); // w = L⁻ᵀL⁻¹ r
        syomv_neg(tril(K), w, z);      // z -= K L⁻ᵀL⁻¹ r
    }
    // Jacobi: z = L⁻ᵀL⁻¹ r
    trsm(tril(L), z);
    trsm(triu(L.transposed()), z);
    return compact_blas::xdot(simdify(r), simdify(z));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_pcg(mut_batch_view<> λ,
                                                   mut_batch_view<> work_pcg) const {
    auto r = work_pcg.middle_cols(0, 1), z = work_pcg.middle_cols(1, 1),
         p = work_pcg.middle_cols(2, 1), Mp = work_pcg.middle_cols(3, 1);
    auto M = pcr_L.batch(0), K = cr_Y.batch(0);
    value_type rᵀz = [&] {
        GUANAQO_TRACE("PCG", 0);
        copy(λ, r);
        fill(value_type{}, λ);
        value_type rᵀz = mul_precond(r, z, Mp, M, K);
        copy(z, p);
        return rᵀz;
    }();
    const auto ε2 = pcg_tolerance * pcg_tolerance;
    for (index_t it = 0; it < pcg_max_iter; ++it) {
        GUANAQO_TRACE("PCG", it + 1);
        value_type pᵀMp = mul_Mv(p, Mp, M, K);
        value_type α    = rᵀz / pᵀMp;
        compact_blas::xaxpy(+α, simdify(p), simdify(λ));
        compact_blas::xaxpy(-α, simdify(Mp), simdify(r));
        value_type r2 = compact_blas::xdot(simdify(r), simdify(r));
        if (pcg_print_resid)
            std::println("{:>4}) pcg resid = {:15.6e}", it, std::sqrt(r2));
        if (r2 < ε2)
            break;
        value_type rᵀz_new = mul_precond(r, z, Mp, M, K);
        value_type β       = rᵀz_new / rᵀz;
        compact_blas::xaxpby(1, simdify(z), β, simdify(p));
        rᵀz = rᵀz_new;
    }
}

} // namespace CYQLONE_NS(cyqlone)::v2
