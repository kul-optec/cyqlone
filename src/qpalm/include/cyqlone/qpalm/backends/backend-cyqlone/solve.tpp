#pragma once

#include <cyqlone/linalg.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>

#include <iostream>

/** 
 * @file
 * Functions for the factorization and solution of Newton system in QPALM for the Cyqlone backend.
 */

namespace CYQLONE_NS(cyqlone::qpalm) {

template <index_t VL, StorageOrder DefaultOrder>
void CyQPALMBackend<VL, DefaultOrder>::solve(Context &ctx, [[maybe_unused]] const var_vec_t &x,
                                             const var_vec_t &grad, const var_vec_t &Mᵀλ,
                                             const var_vec_t &Aᵀŷ, const eq_constr_vec_t &Mxb,
                                             real_t S, [[maybe_unused]] const ineq_constr_vec_t &Σ,
                                             const active_set_t &J, //
                                             var_vec_t &d, var_vec_t &ξ, ineq_constr_vec_t &Ad,
                                             eq_constr_vec_t &Δλ, var_vec_t &MᵀΔλ) {
    const auto rhs = [&](auto, auto, auto di, auto gi, auto Mᵀλi, auto Aᵀŷi, auto Mxbi, auto Δλi) {
        linalg::axpy<0>(di, {-1, -1, -1}, gi, Mᵀλi, Aᵀŷi);
        batmat::linalg::copy(Mxbi, Δλi);
    };
    ocp.foreach_stage(ctx, rhs, d, grad, Mᵀλ, Aᵀŷ, Mxb, Δλ);
    if (settings.print_residuals)
        print_solve_rhs_norms(ctx, d, Δλ, grad, Mᵀλ, Aᵀŷ);
    if (reset_factorization) {
        auto t = get_timed(&Timings::factor);
        ocp.factor_solve(ctx, S, J, d, Δλ); // (d, Δλ) ← L⁻¹ (d, Δλ)
        if (ctx.is_master()) {
            // No synchronization needed here, barriers in factor_solve and solve_reverse
            reset_factorization = false;
            num_updates         = 0;
            ++stats.num_factor;
        }
    } else { // (d, Δλ) ← L⁻¹ (d, Δλ)
        auto t = get_timed(&Timings::solve);
        ocp.solve_forward(ctx, d, Δλ);
    }
    { // (d, Δλ) ← L⁻ᵀ (d, Δλ)
        auto t = get_timed(&Timings::solve);
        ocp.solve_reverse(ctx, d, Δλ);
    }
    { // MᵀΔλ ← Mᵀ Δλ
        auto t = get_timed(&Timings::solve_MT);
        mat_vec_MT(ctx, Δλ, MᵀΔλ);
    }
    { // Ad ← A d
        auto t = get_timed(&Timings::solve_A);
        mat_vec_A(ctx, d, Ad);
    }
    { // ξ ← Q d + S⁻¹ d
        auto t = get_timed(&Timings::solve_grad);
        ocp.cost_gradient(ctx, d, 1 / S, d, 0, ξ);
    }
    if (settings.print_residuals)
        print_solve_resid_norms(ctx, x, d, grad, ξ, Mᵀλ, Aᵀŷ, MᵀΔλ, Ad, J);
}

template <index_t VL, StorageOrder DefaultOrder>
void CyQPALMBackend<VL, DefaultOrder>::print_solve_rhs_norms(Context &ctx, const var_vec_t &d,
                                                             const eq_constr_vec_t &Δλ,
                                                             const var_vec_t &grad,
                                                             const var_vec_t &Mᵀλ,
                                                             const var_vec_t &Aᵀŷ) const {
    int prec                   = settings.print_precision;
    auto grad_norm             = norm_inf_l1_sq(ctx, d);
    auto constr_norm           = norm_inf_l1_sq(ctx, Δλ);
    auto cost_grad_norm        = norm_inf_l1_sq(ctx, grad);
    auto eq_constr_grad_norm   = norm_inf_l1_sq(ctx, Mᵀλ);
    auto ineq_constr_grad_norm = norm_inf_l1_sq(ctx, Aᵀŷ);
    if (ctx.is_master()) {
        std::cout << "                   gradient:    abs∞="
                  << guanaqo::float_to_str(grad_norm.norm_inf(), prec)
                  << ",  abs₂=" << guanaqo::float_to_str(grad_norm.norm_2(), prec)
                  << "      {grad cost=" << guanaqo::float_to_str(cost_grad_norm.norm_2())
                  << ",  Mᵀλ=" << guanaqo::float_to_str(eq_constr_grad_norm.norm_2())
                  << ",  Aᵀŷ=" << guanaqo::float_to_str(ineq_constr_grad_norm.norm_2()) << "}\n"
                  << "                constraints:    abs∞="
                  << guanaqo::float_to_str(constr_norm.norm_inf(), prec)
                  << ",  abs₂=" << guanaqo::float_to_str(constr_norm.norm_2(), prec) << "\n";
    }
}

template <index_t VL, StorageOrder DefaultOrder>
void CyQPALMBackend<VL, DefaultOrder>::print_solve_resid_norms(
    Context &ctx, const var_vec_t &x, const var_vec_t &d, const var_vec_t &grad, const var_vec_t &ξ,
    const var_vec_t &Mᵀλ, const var_vec_t &Aᵀŷ, const var_vec_t &MᵀΔλ, const ineq_constr_vec_t &Ad,
    const active_set_t &J) {
    // allocate workspaces
    if (temp_var.size() == 0 || temp_eq.size() == 0 || temp_ineq.size() == 0)
        ctx.run_single_sync([this] {
            temp_var  = var_vec();
            temp_eq   = eq_constr_vec();
            temp_ineq = ineq_constr_vec();
        });
    auto tm  = get_timed(&Timings::solve_resid);
    int prec = settings.print_precision;
    using batmat::datapar::hmax;
    using std::abs;
    using std::isfinite;
    using std::max;
    using std::sqrt;
    // Compute the product Σ ⊙ (A d) and then Aᵀ(Σ ⊙ (A d))
    const auto ΣAd = [&](auto, auto, auto Ji, auto Adi, auto temp_ineqi) {
        linalg::hadamard(Ji, Adi, temp_ineqi);
    };
    ocp.foreach_stage(ctx, ΣAd, J, Ad, temp_ineq);
    auto &res = temp_var;
    mat_vec_AT(ctx, temp_ineq, res);
    // Compute the augmented Lagrangian gradient norms
    real_t r_norm_sq = 0, grad_norm_sq = 0, r_norm_inf = 0;
    real_t r_kkt_norm_sq = 0, r_kkt_norm_inf = 0;
    const auto resid_simd = [&](auto gradi, auto ξi, auto Mᵀλi, auto Aᵀŷi, auto MᵀΔλi, auto ri) {
        auto gi      = NeumaierSum(gradi) + Mᵀλi + Aᵀŷi;
        simd r_kkt_i = gi + MᵀΔλi + ξi;
        ri += gi + MᵀΔλi + ξi;
        r_norm_inf = max(r_norm_inf, hmax(abs(ri)));
        r_norm_sq += reduce(ri * ri);
        r_kkt_norm_inf = max(r_kkt_norm_inf, hmax(abs(r_kkt_i)));
        r_kkt_norm_sq += reduce(r_kkt_i * r_kkt_i);
        grad_norm_sq += reduce(simd{gi} * simd{gi});
        return ri;
    };
    const auto resid_batch = [&](auto, auto, auto gradi, auto ξi, auto Mᵀλi, auto Aᵀŷi, auto MᵀΔλi,
                                 auto resi) {
        linalg::transform_elementwise(resid_simd, resi, //
                                      gradi, ξi, Mᵀλi, Aᵀŷi, MᵀΔλi, resi);
    };
    ocp.foreach_stage(ctx, resid_batch, grad, ξ, Mᵀλ, Aᵀŷ, MᵀΔλ, res);
    r_norm_sq    = ctx.reduce(r_norm_sq);
    grad_norm_sq = ctx.reduce(grad_norm_sq);
    r_norm_inf   = ctx.reduce(r_norm_inf, [](auto a, auto b) { return max(a, b); });
    if (!isfinite(r_norm_sq))
        r_norm_inf = r_norm_sq;
    if (ctx.is_master())
        std::cout << "        RESID(stationarity inner):   abs∞="
                  << guanaqo::float_to_str(r_norm_inf, prec)
                  << ",  abs₂=" << guanaqo::float_to_str(sqrt(r_norm_sq), prec)
                  << ",  rel₂=" << guanaqo::float_to_str(sqrt(r_norm_sq / grad_norm_sq), prec)
                  << "\n"
                  << "        RESID(stationarity outer):   abs∞="
                  << guanaqo::float_to_str(r_kkt_norm_inf, prec)
                  << ",  abs₂=" << guanaqo::float_to_str(sqrt(r_kkt_norm_sq), prec)
                  << ",  rel₂=" << guanaqo::float_to_str(sqrt(r_kkt_norm_sq / grad_norm_sq), prec)
                  << "\n";
    auto &x_next        = temp_var;
    const auto add_step = [&](auto, auto, auto xi, auto dii, auto x_nexti) {
        linalg::add(xi, dii, x_nexti);
    };
    ocp.foreach_stage(ctx, add_step, x, d, x_next);
    auto &res_x_next = temp_eq;
    eq_constr_resid(ctx, x_next, res_x_next);
    real_t inf_res = norm_inf(ctx, res_x_next);
    if (ctx.is_master())
        std::cout << "        RESID(eq. feasibility):  abs∞="
                  << guanaqo::float_to_str(inf_res, prec) << "\n";
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
