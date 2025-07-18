#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/symm.hpp>
#include <batmat/linalg/uview.hpp>
#include <batmat/loop.hpp>

namespace cyqlone {
using batmat::linalg::simdify;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::residual_dynamics_constr(view<> x, view<> b,
                                                                    mut_view<> Mxb) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        {
            GUANAQO_TRACE("resid_dyn_constr init", k0);
            index_t ti_next = add_wrap_PmV(ti, 1);
            index_t di_next = ti_next * num_stages + num_stages - 1;
            auto x_next     = x.batch(di_next).bottom_rows(nx);
            auto Mxb0       = Mxb.batch(di0);
            auto b0         = b.batch(di0);

            ti_next == 0 ? compact_blas::template xadd_neg_copy<-1>(simdify(Mxb0), simdify(x_next),
                                                                    simdify(b0))
                         : compact_blas::template xadd_neg_copy<0>(simdify(Mxb0), simdify(x_next),
                                                                   simdify(b0));
        }
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("resid_dyn_constr", k);
            index_t di = di0 + i;
            auto BAi   = data_BA.batch(di);
            auto uxi   = x.batch(di);
            gemm_add(BAi, uxi, Mxb.batch(di));
            if (i + 1 < num_stages) {
                index_t di_next = di + 1;
                compact_blas::xadd_neg_copy(simdify(Mxb.batch(di_next)), simdify(b.batch(di_next)),
                                            simdify(uxi.bottom_rows(nx)));
            }
        }
    });
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::transposed_dynamics_constr(view<> λ,
                                                                      mut_view<> Mᵀλ) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("transposed_dynamics_constr", k);
            index_t di = di0 + i;
            auto BAi   = data_BA.batch(di);
            Mᵀλ.batch(di).top_rows(nu).set_constant(0);
            if (i + 1 < num_stages) {
                index_t di_next = di + 1;
                compact_blas::xadd_neg_copy(simdify(Mᵀλ.batch(di).bottom_rows(nx)),
                                            simdify(λ.batch(di_next)));
            } else {
                const index_t ti_next = sub_wrap_PmV(ti, 1);
                index_t di_next       = ti_next * num_stages;
                ti == 0 ? compact_blas::template xadd_neg_copy<1>(
                              simdify(Mᵀλ.batch(di).bottom_rows(nx)), simdify(λ.batch(di_next)))
                              : compact_blas::template xadd_neg_copy<0>(
                              simdify(Mᵀλ.batch(di).bottom_rows(nx)), simdify(λ.batch(di_next)));
            }
            gemm_add(BAi.transposed(), λ.batch(di), Mᵀλ.batch(di));
        }
    });
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::general_constr(view<> ux, mut_view<> DCux) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("general_constr", k);
            index_t di = di0 + i;
            gemm(data_DCᵀ.batch(di).transposed(), ux.batch(di), DCux.batch(di));
        }
    });
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::transposed_general_constr(view<> y,
                                                                     mut_view<> DCᵀy) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("transposed_general_constr", k);
            index_t di = di0 + i;
            gemm(data_DCᵀ.batch(di), y.batch(di), DCᵀy.batch(di));
        }
    });
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::cost_gradient(view<> ux, value_type a, view<> q,
                                                         value_type b, mut_view<> grad_f) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("cost_gradient", k);
            index_t di = di0 + i;
            if (a != 0 || b != 1)
                compact_blas::xaxpby(a, simdify(q.batch(di)), b, simdify(grad_f.batch(di)));
            symm_add(tril(data_RSQ.batch(di)), ux.batch(di), grad_f.batch(di));
        }
    });
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::cost_gradient_regularized(value_type S, view<> ux,
                                                                     view<> ux0, view<> q,
                                                                     mut_view<> grad_f) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        using abi        = batmat::linalg::simdified_abi_t<decltype(ux.batch(0))>;
        using simd_types = batmat::linalg::simd_view_types<T, abi>;
        using simd       = simd_types::simd;
        simd invS{1 / S};
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("cost_gradient_regularized", k);
            index_t di = di0 + i;
            auto qi = q.batch(di), xi = ux.batch(di), x0i = ux0.batch(di);
            auto grad_fi = grad_f.batch(di);
            for (index_t j = 0; j < ux.rows(); ++j) {
                simd qij      = simd_types::aligned_load(&qi(0, j, 0)),
                     xij      = simd_types::aligned_load(&xi(0, j, 0)),
                     x0ij     = simd_types::aligned_load(&x0i(0, j, 0));
                simd grad_fij = invS * (xij - x0ij) + qij;
                simd_types::aligned_store(grad_fij, &grad_fi(0, j, 0));
            }
            symm_add(tril(data_RSQ.batch(di)), ux.batch(di), grad_f.batch(di));
        }
    });
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::cost_gradient_remove_regularization(
    value_type S, view<> ux, view<> ux0, mut_view<> grad_f) const {
    const index_t P = 1 << (lP - lvl);
    batmat::foreach_thread(P, [&](index_t ti, index_t) {
        using abi        = batmat::linalg::simdified_abi_t<decltype(ux.batch(0))>;
        using simd_types = batmat::linalg::simd_view_types<T, abi>;
        using simd       = simd_types::simd;
        simd invS{1 / S};
        const index_t num_stages = ceil_N >> lP;    // number of stages per thread
        const index_t di0        = ti * num_stages; // data batch index
        const index_t k0         = ti * num_stages; // stage index
        for (index_t i = 0; i < num_stages; ++i) {
            [[maybe_unused]] index_t k = sub_wrap_N(k0, i);
            GUANAQO_TRACE("cost_gradient_remove_regularization", k);
            index_t di = di0 + i;
            auto xi = ux.batch(di), x0i = ux0.batch(di);
            auto grad_fi = grad_f.batch(di);
            for (index_t j = 0; j < ux.rows(); ++j) {
                simd grad_fij = simd_types::aligned_load(&grad_fi(0, j, 0)),
                     xij      = simd_types::aligned_load(&xi(0, j, 0)),
                     x0ij     = simd_types::aligned_load(&x0i(0, j, 0));
                grad_fij += invS * (x0ij - xij);
                simd_types::aligned_store(grad_fij, &grad_fi(0, j, 0));
            }
        }
    });
}

} // namespace cyqlone
