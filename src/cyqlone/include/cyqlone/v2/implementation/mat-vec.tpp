#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/symv.hpp>
#include <batmat/linalg/uview.hpp>
#include <batmat/loop.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {

using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::residual_dynamics_constr(Context &ctx, view<> x, view<> b,
                                                                  mut_view<> Mxb) const {
    // (Mx + b)(j) = A(j) x(j) + B(j) u(j) - x(j+1) + b(j)
    auto arrival          = ctx.arrive();
    const index_t c       = ctx.index;
    const index_t dn      = c * n; // data batch index
    const index_t jn      = c * n; // stage index
    const index_t c_next  = add_wrap_p(c, 1);
    const index_t dn_next = c_next * n, d1_next = dn_next + n - 1;
    for (index_t i = n; i-- > 0;) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("resid_dyn_constr", j);
        index_t di = dn + i;
        auto BAj   = data_F.batch(di);
        auto uxj   = x.batch(di);
        auto bj    = b.batch(di);
        auto Mxbj  = Mxb.batch(di);
        gemv_add(BAj, uxj, bj, Mxbj); // A(j) x(j) + B(j) u(j) + b(j)
        if (i > 0) {
            index_t di_next = di - 1; // j + 1
            auto x_next     = x.batch(di_next).bottom_rows(nx);
            compact_blas::xsub(simdify(Mxbj), simdify(x_next)); // - x(j+1)
        } else {
            ctx.wait(std::move(arrival)); // x_next comes from next thread
            auto x_next = x.batch(d1_next).bottom_rows(nx);
            if (c_next > 0 || VL == 1)
                compact_blas::template xsub<+0>(simdify(Mxbj), simdify(x_next));
            else
                compact_blas::template xsub<-1>(simdify(Mxbj), simdify(x_next));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::transposed_dynamics_constr(Context &ctx, view<> λ,
                                                                    mut_view<> Mᵀλ,
                                                                    bool accum) const {
    // (Mᵀλ)(j) = [ B(j)ᵀ ] λ(j) - [ 0 ] λ(j-1)
    //            [ A(j)ᵀ ]        [ I ]
    auto arrival          = ctx.arrive();
    const index_t c       = ctx.index;
    const index_t dn      = c * n; // data batch index
    const index_t jn      = c * n; // stage index
    const index_t c_prev  = sub_wrap_p(c, 1);
    const index_t dn_prev = c_prev * n;
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("trans_dyn_constr", j);
        index_t di = dn + i;
        auto BAj = data_F.batch(di), Bj = BAj.left_cols(nu);
        auto λj   = λ.batch(di);
        auto Mᵀλj = Mᵀλ.batch(di);
        if (VL > 1 || c > 0 || i > 0) {
            accum ? gemv_add(BAj.transposed(), λj, Mᵀλj) //
                  : gemv(BAj.transposed(), λj, Mᵀλj);
        } else {
            accum ? gemv_add(Bj.transposed(), λj, Mᵀλj.top_rows(nu)) //
                  : gemv(Bj.transposed(), λj, Mᵀλj.top_rows(nu));
            if (!accum)
                Mᵀλj.bottom_rows(nx).set_constant(0);
        }
        if (i + 1 < n) {
            index_t di_prev = di + 1; // j - 1
            auto λ_prev     = λ.batch(di_prev);
            compact_blas::xsub(simdify(Mᵀλj.bottom_rows(nx)), simdify(λ_prev));
        } else {
            ctx.wait(std::move(arrival)); // λ_prev comes from previous thread
            auto λ_prev = λ.batch(dn_prev);
            if (c > 0 || VL == 1)
                compact_blas::template xsub<0>(simdify(Mᵀλj.bottom_rows(nx)), simdify(λ_prev));
            else
                compact_blas::template xsub<1>(simdify(Mᵀλj.bottom_rows(nx)), simdify(λ_prev));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::general_constr(Context &ctx, view<> ux,
                                                        mut_view<> DCux) const {
    const index_t c  = ctx.index;
    const index_t dn = c * n; // data batch index
    const index_t jn = c * n; // stage index
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("general_constr", j);
        index_t di = dn + i;
        gemv(data_Gᵀ.batch(di).transposed(), ux.batch(di), DCux.batch(di));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::transposed_general_constr(Context &ctx, view<> y,
                                                                   mut_view<> DCᵀy) const {
    const index_t c  = ctx.index;
    const index_t dn = c * n; // data batch index
    const index_t jn = c * n; // stage index
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("transposed_general_constr", j);
        index_t di = dn + i;
        gemv(data_Gᵀ.batch(di), y.batch(di), DCᵀy.batch(di));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::transposed_general_constr(view<> y,
                                                                   mut_view<> DCᵀy) const {
    for (index_t c = 0; c < p; ++c) {
        const index_t dn = c * n; // data batch index
        const index_t jn = c * n; // stage index
        for (index_t i = 0; i < n; ++i) {
            [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
            GUANAQO_TRACE("transposed_general_constr", j);
            index_t di = dn + i;
            gemv(data_Gᵀ.batch(di), y.batch(di), DCᵀy.batch(di));
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::cost_gradient(Context &ctx, view<> ux, value_type a,
                                                       view<> q, value_type b,
                                                       mut_view<> grad_f) const {
    const index_t c  = ctx.index;
    const index_t dn = c * n; // data batch index
    const index_t jn = c * n; // stage index
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("cost_gradient", j);
        index_t di = dn + i;
        if (a != 0 || b != 1)
            compact_blas::xaxpby(a, simdify(q.batch(di)), b, simdify(grad_f.batch(di)));
        symv_add(tril(data_H.batch(di)), ux.batch(di), grad_f.batch(di));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::cost_gradient_regularized(Context &ctx, value_type γ,
                                                                   view<> ux, view<> ux0, view<> q,
                                                                   mut_view<> grad_f) const {
    const index_t c  = ctx.index;
    using abi        = batmat::linalg::simdified_abi_t<decltype(ux.batch(0))>;
    using simd_types = batmat::linalg::simd_view_types<T, abi>;
    using simd       = simd_types::simd;
    simd inv_γ{1 / γ};
    const index_t dn = c * n; // data batch index
    const index_t jn = c * n; // stage index
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("cost_gradient_regularized", j);
        index_t di = dn + i;
        auto qi = q.batch(di), xi = ux.batch(di), x0i = ux0.batch(di);
        auto grad_fi = grad_f.batch(di);
        for (index_t j = 0; j < ux.rows(); ++j) {
            simd qij      = simd_types::aligned_load(&qi(0, j, 0)),
                 xij      = simd_types::aligned_load(&xi(0, j, 0)),
                 x0ij     = simd_types::aligned_load(&x0i(0, j, 0));
            simd grad_fij = inv_γ * (xij - x0ij) + qij;
            simd_types::aligned_store(grad_fij, &grad_fi(0, j, 0));
        }
        symv_add(tril(data_H.batch(di)), ux.batch(di), grad_f.batch(di));
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::cost_gradient_remove_regularization(
    Context &ctx, value_type γ, view<> ux, view<> ux0, mut_view<> grad_f) const {
    const index_t c  = ctx.index;
    using abi        = batmat::linalg::simdified_abi_t<decltype(ux.batch(0))>;
    using simd_types = batmat::linalg::simd_view_types<T, abi>;
    using simd       = simd_types::simd;
    simd inv_γ{1 / γ};
    const index_t dn = c * n; // data batch index
    const index_t jn = c * n; // stage index
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_N(jn, i);
        GUANAQO_TRACE("cost_gradient_remove_regularization", j);
        index_t di = dn + i;
        auto xi = ux.batch(di), x0i = ux0.batch(di);
        auto grad_fi = grad_f.batch(di);
        for (index_t j = 0; j < ux.rows(); ++j) {
            simd grad_fij = simd_types::aligned_load(&grad_fi(0, j, 0)),
                 xij      = simd_types::aligned_load(&xi(0, j, 0)),
                 x0ij     = simd_types::aligned_load(&x0i(0, j, 0));
            grad_fij += inv_γ * (x0ij - xij);
            simd_types::aligned_store(grad_fij, &grad_fi(0, j, 0));
        }
    }
}

} // namespace CYQLONE_NS(cyqlone)::v2
