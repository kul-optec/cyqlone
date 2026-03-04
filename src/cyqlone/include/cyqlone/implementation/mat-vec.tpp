#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/linalg/symv.hpp>
#include <batmat/linalg/uview.hpp>
#include <batmat/loop.hpp>

namespace CYQLONE_NS(cyqlone) {

using namespace linalg;
using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::residual_dynamics_constr(Context &ctx, view<> x,
                                                                       view<> b,
                                                                       mut_view<> Mxb) const {
    // (Mx + b)(j) = A(j) x(j) + B(j) u(j) - x(j+1) + b(j)
    auto arrival          = ctx.arrive();
    const index_t c       = riccati_thread_assignment(ctx);
    const index_t dn      = c * n; // data batch index
    const index_t jn      = c * n; // stage index
    const index_t c_next  = add_wrap_p(c, 1);
    const index_t dn_next = c_next * n, d1_next = dn_next + n - 1;
    for (index_t i = n; i-- > 0;) {
        [[maybe_unused]] index_t j = sub_wrap_ceil_N(jn, i);
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
            sub(Mxbj, x_next); // - x(j+1)
        } else {
            ctx.wait(std::move(arrival)); // x_next comes from next thread
            auto x_next = x.batch(d1_next).bottom_rows(nx);
            if (c_next > 0 || v == 1)
                sub(Mxbj, x_next);
            else
                sub(Mxbj, x_next, with_rotate<1>);
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::transposed_dynamics_constr(Context &ctx, view<> λ,
                                                                         mut_view<> Mᵀλ,
                                                                         bool accum) const {
    // (Mᵀλ)(j) = [ B(j)ᵀ ] λ(j) - [ 0 ] λ(j-1)
    //            [ A(j)ᵀ ]        [ I ]
    auto arrival          = ctx.arrive();
    const index_t c       = riccati_thread_assignment(ctx);
    const index_t dn      = c * n; // data batch index
    const index_t jn      = c * n; // stage index
    const index_t c_prev  = sub_wrap_p(c, 1);
    const index_t dn_prev = c_prev * n;
    for (index_t i = 0; i < n; ++i) {
        [[maybe_unused]] index_t j = sub_wrap_ceil_N(jn, i);
        GUANAQO_TRACE("trans_dyn_constr", j);
        index_t di = dn + i;
        auto BAj = data_F.batch(di), Bj = BAj.left_cols(nu);
        auto λj   = λ.batch(di);
        auto Mᵀλj = Mᵀλ.batch(di);
        if (v > 1 || c > 0 || i > 0) {
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
            sub(Mᵀλj.bottom_rows(nx), λ_prev);
        } else {
            ctx.wait(std::move(arrival)); // λ_prev comes from previous thread
            auto λ_prev = λ.batch(dn_prev);
            if (c > 0 || v == 1)
                sub(Mᵀλj.bottom_rows(nx), λ_prev);
            else
                sub(Mᵀλj.bottom_rows(nx), λ_prev, with_rotate<-1>);
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::general_constr(Context &ctx, view<> ux,
                                                             mut_view<> DCux) const {
    const auto mul_Gx = []([[maybe_unused]] auto j, auto, auto Gᵀj, auto uxj, auto DCuxj) {
        GUANAQO_TRACE("general_constr", j);
        gemv(Gᵀj.transposed(), uxj, DCuxj);
    };
    foreach_stage(ctx, mul_Gx, data_Gᵀ, ux, DCux);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::transposed_general_constr(Context &ctx, view<> y,
                                                                        mut_view<> DCᵀy) const {
    const auto mul_Gᵀy = []([[maybe_unused]] auto j, auto, auto Gᵀj, auto yj, auto DCᵀyj) {
        GUANAQO_TRACE("transposed_general_constr", j);
        gemv(Gᵀj, yj, DCᵀyj);
    };
    foreach_stage(ctx, mul_Gᵀy, data_Gᵀ, y, DCᵀy);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::cost_gradient(Context &ctx, view<> ux, value_type α,
                                                            view<> q, value_type β,
                                                            mut_view<> grad_f) const {
    const auto mul_Hx = [&]([[maybe_unused]] auto j, auto, auto qj, auto Hj, auto uxj,
                            auto grad_fj) {
        GUANAQO_TRACE("cost_gradient", j);
        if (α != 0 || β != 1)
            axpby(α, qj, β, grad_fj);
        symv_add(tril(Hj), uxj, grad_fj);
    };
    foreach_stage(ctx, mul_Hx, q, data_H, ux, grad_f);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::cost_gradient_regularized(Context &ctx, value_type γ,
                                                                        view<> ux, view<> ux0,
                                                                        view<> q,
                                                                        mut_view<> grad_f) const {
    simd inv_γ{1 / γ};
    const auto reg_simd = [inv_γ](auto qji, auto xji, auto x0ji) {
        return inv_γ * (xji - x0ji) + qji;
    };
    const auto mul_Hx = [&]([[maybe_unused]] auto j, auto, auto qj, auto Hj, auto uxj, auto ux0j,
                            auto grad_fj) {
        GUANAQO_TRACE("cost_gradient_regularized", j);
        linalg::transform_elementwise(reg_simd, grad_fj, qj, uxj, ux0j);
        symv_add(tril(Hj), uxj, grad_fj);
    };
    foreach_stage(ctx, mul_Hx, q, data_H, ux, ux0, grad_f);
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
void CyqloneSolver<VL, T, DefaultOrder, Ctx>::cost_gradient_remove_regularization(
    Context &ctx, value_type γ, view<> ux, view<> ux0, mut_view<> grad_f) const {
    simd inv_γ{1 / γ};
    const auto sub_reg_simd = [inv_γ](auto grad_fji, auto xji, auto x0ji) {
        return grad_fji + inv_γ * (x0ji - xji);
    };
    const auto sub_reg = [&]([[maybe_unused]] auto j, auto, auto uxj, auto ux0j, auto grad_fj) {
        GUANAQO_TRACE("cost_gradient_remove_regularization", j);
        linalg::transform_elementwise(sub_reg_simd, grad_fj, grad_fj, uxj, ux0j);
    };
    foreach_stage(ctx, sub_reg, ux, ux0, grad_f);
}

} // namespace CYQLONE_NS(cyqlone)
