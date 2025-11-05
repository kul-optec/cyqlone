#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <print>

#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/syomv.hpp>
#include <batmat/linalg/trsm.hpp>

namespace CYQLONE_NAMESPACE {
using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::mul_A(batch_view<> p, mut_batch_view<> Ap,
                                               batch_view<default_order> L,
                                               batch_view<default_order> B) const -> value_type {
    copy(p, Ap);
    trmm(tril(L).transposed(), Ap);
    trmm(tril(L), Ap);
    syomv(tril(B), p, Ap);
    return compact_blas::xdot(simdify(p), simdify(Ap));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::mul_precond(batch_view<> r, mut_batch_view<> z,
                                                     mut_batch_view<> w,
                                                     batch_view<default_order> L,
                                                     batch_view<default_order> B) const
    -> value_type {
    copy(r, z);
    if (use_stair_preconditioner) {
        copy(r, w);
        trsm(tril(L), w);
        trsm(tril(L).transposed(), w);
        syomv_neg(tril(B), w, z);
    }
    trsm(tril(L), z);
    trsm(tril(L).transposed(), z);
    return compact_blas::xdot(simdify(r), simdify(z));
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::solve_pcg(mut_batch_view<> λ,
                                                   mut_batch_view<> work_pcg) const {
    auto r = work_pcg.middle_cols(0, 1), z = work_pcg.middle_cols(1, 1),
         p = work_pcg.middle_cols(2, 1), Ap = work_pcg.middle_cols(3, 1);
    auto A = coupling_D.batch(0), B = coupling_Y.batch(0);
    value_type rᵀz = [&] {
        GUANAQO_TRACE("solve Ψ pcg", 0);
        copy(λ, r);
        fill(value_type{}, λ);
        value_type rᵀz = mul_precond(r, z, Ap, A, B);
        copy(z, p);
        return rᵀz;
    }();
    const auto ε2 = pcg_tolerance * pcg_tolerance;
    for (index_t it = 0; it < pcg_max_iter; ++it) { // TODO
        GUANAQO_TRACE("solve Ψ pcg", it + 1);
        value_type pᵀAp = mul_A(p, Ap, A, B);
        value_type α    = rᵀz / pᵀAp;
        compact_blas::xaxpy(+α, simdify(p), simdify(λ));
        compact_blas::xaxpy(-α, simdify(Ap), simdify(r));
        value_type r2 = compact_blas::xdot(simdify(r), simdify(r));
        if (pcg_print_resid)
            std::println("{:>4}) pcg resid = {:15.6e}", it, std::sqrt(r2));
        if (r2 < ε2)
            break;
        value_type rᵀz_new = mul_precond(r, z, Ap, A, B);
        value_type β       = rᵀz_new / rᵀz;
        compact_blas::xaxpby(1, simdify(z), β, simdify(p));
        rᵀz = rᵀz_new;
    }
}

} // namespace CYQLONE_NAMESPACE
