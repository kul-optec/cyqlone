#pragma once

#include <cyqlone/linalg.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>

namespace CYQLONE_NS(cyqlone::qpalm) {

template <index_t VL, StorageOrder DefaultOrder>
index_t CyQPALMBackend<VL, DefaultOrder>::update_penalty_y(Context &ctx, ineq_constr_vec_t &Σ,
                                                           const ineq_constr_vec_t &e,
                                                           const ineq_constr_vec_t &e_old,
                                                           const PenaltySettings &settings) {
    GUANAQO_TRACE("update_penalty_y", 0);
    auto t                  = get_timed(&Timings::update_penalty_y);
    const real_t min_denom  = 1e-6;
    const real_t norm_inf_e = fmax(min_denom, norm_inf(ctx, e));
    index_t num_changed     = 0;
    const auto update_simd  = [&](auto Σji, auto eji, auto eji_old) {
        auto insufficient_progress = abs(eji) >= settings.θ * abs(eji_old);
#if BATMAT_WITH_GSI_HPC_SIMD
        simd update_factor =
            select(insufficient_progress, settings.Δy * abs(eji) / norm_inf_e, simd{1});
#else
        simd update_factor{1};
        where(insufficient_progress, update_factor) = settings.Δy * abs(eji) / norm_inf_e;
#endif
        update_factor *= settings.Δy_always;
        auto Σ_new = Σji * update_factor;
        Σ_new      = fmax(Σji * settings.Δy_always, fmin(Σ_new, simd{settings.max_penalty_y}));
#if BATMAT_WITH_GSI_HPC_SIMD
        num_changed += reduce_count(Σ_new != Σji);
#else
        num_changed += popcount(Σ_new != Σji);
#endif
        return Σ_new;
    };
    const auto update_batch = [&](auto, auto, auto Σj, auto ej, auto ej_old) {
        linalg::transform_elementwise(update_simd, Σj, Σj, ej, ej_old);
    };
    ocp.foreach_stage(ctx, update_batch, Σ, e, e_old);
    return ctx.reduce(num_changed);
}

template <index_t VL, StorageOrder DefaultOrder>
void CyQPALMBackend<VL, DefaultOrder>::ineq_constr_resid(Context &ctx, const ineq_constr_vec_t &Ax,
                                                         ineq_constr_vec_t &e) const {
    GUANAQO_TRACE("ineq_constr_resid", 0);
    auto t                       = get_timed(&Timings::ineq_constr_resid);
    const auto ineq_constr_resid = [](auto, auto, auto Axj, auto b_minj, auto b_maxj, auto ej) {
        linalg::clamp_resid(Axj, b_minj, b_maxj, ej);
    };
    ocp.foreach_stage(ctx, ineq_constr_resid, Ax, b_min_strided, b_max_strided, e);
}

template <index_t VL, StorageOrder DefaultOrder>
void CyQPALMBackend<VL, DefaultOrder>::project_multipliers_ineq(Context &ctx,
                                                                ineq_constr_vec_t &y) const {
    GUANAQO_TRACE("project_multipliers_ineq", 0);
    const auto proj_simd = [](auto yji, auto b_minji, auto b_maxji) {
#if BATMAT_WITH_GSI_HPC_SIMD
        yji = select(isfinite(b_minji), yji, fmax(yji, simd{0}));
        yji = select(isfinite(b_maxji), yji, fmin(yji, simd{0}));
#else
        where(!isfinite(b_minji), yji) = fmax(yji, simd{0});
        where(!isfinite(b_maxji), yji) = fmin(yji, simd{0});
#endif
        return yji;
    };
    const auto proj_batch = [&](auto, auto, auto yji, auto b_minji, auto b_maxji) {
        linalg::transform_elementwise(proj_simd, yji, yji, b_minji, b_maxji);
    };
    ocp.foreach_stage(ctx, proj_batch, y, b_min_strided, b_max_strided);
}

template <index_t VL, StorageOrder DefaultOrder>
real_t CyQPALMBackend<VL, DefaultOrder>::ineq_constr_viol(Context &ctx,
                                                          const ineq_constr_vec_t &Ax) const {
    GUANAQO_TRACE("ineq_constr_viol", 0);
    auto t               = get_timed(&Timings::ineq_constr_viol);
    auto nrm_simd        = norms.zero_simd();
    const auto viol_simd = [&](auto Axji, auto b_minji, auto b_maxji) {
        auto zi  = fmax(b_minji, fmin(Axji, b_maxji));
        nrm_simd = norms(nrm_simd, Axji - zi);
    };
    const auto viol_batch = [&](auto, auto, auto Axj, auto b_min_j, auto b_max_j) {
        linalg::for_each_elementwise(viol_simd, Axj, b_min_j, b_max_j);
    };
    ocp.foreach_stage(ctx, viol_batch, Ax, b_min_strided, b_max_strided);
    return ctx.reduce(norms(nrm_simd), norms).norminf();
}

template <index_t VL, StorageOrder DefaultOrder>
real_t CyQPALMBackend<VL, DefaultOrder>::ineq_constr_resid_al(Context &ctx,
                                                              const ineq_constr_vec_t &y,
                                                              const ineq_constr_vec_t &ŷ,
                                                              const ineq_constr_vec_t &Σ,
                                                              ineq_constr_vec_t &e) {
    GUANAQO_TRACE("ineq_constr_resid_al", 0);
    auto t                = get_timed(&Timings::ineq_constr_resid_al);
    auto nrm_simd         = norms.zero_simd();
    const auto resid_simd = [&](auto yji, auto ŷji, auto Σji) {
        auto ei  = (ŷji - yji) / Σji;
        nrm_simd = norms(nrm_simd, ei);
        return ei;
    };
    const auto resid_batch = [&](auto, auto, auto ej, auto yj, auto ŷj, auto Σj) {
        linalg::transform_elementwise(resid_simd, ej, yj, ŷj, Σj);
    };
    ocp.foreach_stage(ctx, resid_batch, e, y, ŷ, Σ);
    return ctx.reduce(norms(nrm_simd), norms).norminf();
}

template <index_t VL, StorageOrder DefaultOrder>
index_t CyQPALMBackend<VL, DefaultOrder>::calc_ŷ_Aᵀŷ(Context &ctx, const ineq_constr_vec_t &Ax,
                                                     const ineq_constr_vec_t &Σ,
                                                     const ineq_constr_vec_t &y,
                                                     ineq_constr_vec_t &ŷ, var_vec_t &Aᵀŷ,
                                                     active_set_t &J) {
    index_t count_J   = 0;
    const auto ŷ_simd = [&count_J](auto Σi, auto yi, auto Axi, auto li, auto ui) {
        auto ζ = Axi + yi / Σi, z = fmax(li, fmin(ζ, ui));
        auto Ji = z != ζ; // TODO: inclusive?
#if 0
            simd ŷi{0};
            where(Ji, ŷi) = yi + Σi * (Axi - z);
#else
        auto ŷi = yi + Σi * (Axi - z);
#endif
#if BATMAT_WITH_GSI_HPC_SIMD
        simd ΣJi = select(Ji, Σi, simd{0});
        count_J += static_cast<index_t>(reduce_count(Ji));
#else
        simd ΣJi{};
        where(Ji, ΣJi) = Σi;
        count_J += static_cast<index_t>(popcount(Ji));
#endif
        return std::make_pair(ŷi, ΣJi);
    };
    const auto ŷ_batch = [&ŷ_simd]([[maybe_unused]] auto j, auto, auto ŷi, auto Ji, auto Σi,
                                   auto yi, auto Axi, auto li, auto ui) {
        GUANAQO_TRACE("calc_ŷ_Aᵀŷ", j);
        linalg::transform2_elementwise(ŷ_simd, ŷi, Ji, //
                                       Σi, yi, Axi, li, ui);
    };
    {
        auto t = get_timed(&Timings::calc_y_hat);
        ocp.foreach_stage(ctx, ŷ_batch, ŷ, J, Σ, y, Ax, b_min_strided, b_max_strided);
    }
    auto t = get_timed(&Timings::calc_y_hat_AT);
    mat_vec_AT(ctx, ŷ, Aᵀŷ);
    return ctx.reduce(count_J);
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
