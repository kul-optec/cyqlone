#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone.hpp>
#include <cyqlone/neumaier.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/implementation/breakpoint.hpp>
#include <cyqlone/reduce.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/simdify.hpp>
#include <batmat/openmp.h>
#include <batmat/simd.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/timed-cpu.hpp>
#include <guanaqo/trace.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <tuple>
#include <utility>

namespace CYQLONE_NS(cyqlone::qpalm) {

using batmat::linalg::simdify;
namespace datapar = batmat::datapar;

template <index_t VL, StorageOrder DefaultOrder>
struct CyqloneBackend {
    using OCP_t                 = cyqlone::CyqloneSolver<VL, real_t, DefaultOrder>;
    using Context               = typename OCP_t::Context;
    using storage_t             = typename OCP_t::template matrix<>;
    using mask_storage_t        = typename OCP_t::template mask_matrix<>;
    using simd                  = typename OCP_t::compact_blas::simd;
    static constexpr auto norms = cyqlone::norms<real_t, simd>{};
    // clang-format off
    struct var_vec_t         : storage_t { friend CyqloneBackend; var_vec_t() = default;         private: var_vec_t(storage_t &&o)         : storage_t{std::move(o)} {} friend auto simdify(var_vec_t &s) { return batmat::linalg::simdify(static_cast<storage_t &>(s)); } friend auto simdify(const var_vec_t &s) { return batmat::linalg::simdify(static_cast<const storage_t &>(s)); }};
    struct eq_constr_vec_t   : storage_t { friend CyqloneBackend; eq_constr_vec_t() = default;   private: eq_constr_vec_t(storage_t &&o)   : storage_t{std::move(o)} {} friend auto simdify(eq_constr_vec_t &s) { return batmat::linalg::simdify(static_cast<storage_t &>(s)); } friend auto simdify(const eq_constr_vec_t &s) { return batmat::linalg::simdify(static_cast<const storage_t &>(s)); }};
    struct ineq_constr_vec_t : storage_t { friend CyqloneBackend; ineq_constr_vec_t() = default; private: ineq_constr_vec_t(storage_t &&o) : storage_t{std::move(o)} {} friend auto simdify(ineq_constr_vec_t &s) { return batmat::linalg::simdify(static_cast<storage_t &>(s)); } friend auto simdify(const ineq_constr_vec_t &s) { return batmat::linalg::simdify(static_cast<const storage_t &>(s)); }};
    struct active_set_t      : storage_t { friend CyqloneBackend; active_set_t() = default;      private: active_set_t(storage_t &&o)      : storage_t{std::move(o)} {} friend auto simdify(active_set_t &s) { return batmat::linalg::simdify(static_cast<storage_t &>(s)); } friend auto simdify(const active_set_t &s) { return batmat::linalg::simdify(static_cast<const storage_t &>(s)); }};
    // clang-format on

    OCP_t ocp;
    CyqloneBackendSettings settings;
    ineq_constr_vec_t b_min_strided, b_max_strided;
    eq_constr_vec_t b_eq_strided;
    var_vec_t grad_strided;
    ineq_constr_vec_t ΔΣ;
    std::optional<var_vec_t> x0;
    std::optional<ineq_constr_vec_t> y0;
    std::optional<eq_constr_vec_t> λ0;
    std::vector<std::array<size_t, 4>> thread_indices;
    std::vector<ABSum_t> thread_sums;
    std::vector<Breakpoint> breakpoints_temp;

    bool reset_factorization = true;
    index_t num_updates      = 0;
    std::unique_ptr<typename OCP_t::Timings> ocp_timings;

    CyqloneBackend(const CyqloneStorage<> &ocp, CyqloneData data,
                   const CyqloneBackendSettings &settings)
        : ocp{OCP_t::build(ocp, settings.log_processors)}, settings{settings} {
        this->ocp.alt                              = settings.factor_alt;
        this->ocp.pcg_max_iter                     = settings.pcg_max_iter;
        this->ocp.pcg_tolerance                    = settings.pcg_tolerance;
        this->ocp.pcg_print_resid                  = settings.pcg_print_resid;
        this->ocp.solve_method                     = settings.solve_method;
        this->ocp.parallel_ctx->barrier.spin_count = settings.spin_count;
        b_min_strided                              = ineq_constr_vec();
        b_max_strided                              = ineq_constr_vec();
        ΔΣ                                         = ineq_constr_vec();
        b_eq_strided                               = eq_constr_vec();
        grad_strided                               = var_vec();
        this->ocp.initialize_rhs(ocp, b_eq_strided);
        this->ocp.initialize_gradient(ocp, grad_strided);
        this->ocp.initialize_bounds(ocp, b_min_strided, b_max_strided);
        if (!data.initial_variables.empty()) {
            x0 = var_vec();
            this->ocp.pack_variables(data.initial_variables, x0->view());
        }
        if (!data.initial_inequality_multipliers.empty()) {
            y0 = ineq_constr_vec();
            this->ocp.pack_constraints(data.initial_inequality_multipliers, y0->view());
        }
        if (!data.initial_equality_multipliers.empty()) {
            λ0 = eq_constr_vec();
            this->ocp.pack_dynamics(data.initial_equality_multipliers, λ0->view());
        }
        if (settings.detailed_timings)
            ocp_timings = std::make_unique<typename OCP_t::Timings>();
    }

    void update_data(const CyqloneStorage<> &ocp) {
        this->ocp.update_data(ocp);
        this->ocp.initialize_rhs(ocp, b_eq_strided);
        this->ocp.initialize_gradient(ocp, grad_strided);
        this->ocp.initialize_bounds(ocp, b_min_strided, b_max_strided);
    }

    void set_b_eq(std::span<const real_t> b_eq) {
        this->ocp.pack_dynamics(b_eq, b_eq_strided.view());
    }
    void set_b_lb(std::span<const real_t> b_lb) {
        this->ocp.pack_constraints(b_lb, b_min_strided.view());
    }
    void set_b_ub(std::span<const real_t> b_ub) {
        this->ocp.pack_constraints(b_ub, b_max_strided.view());
    }

    void warm_start(const var_vec_t &x, const ineq_constr_vec_t &y, const eq_constr_vec_t &λ) {
        const auto k_to_l = [&](index_t k) {
            const auto num_stages = ocp.ceil_N >> ocp.lP;
            const auto i          = (ocp.ceil_N - k) % num_stages;
            const auto k1         = (k + i) / num_stages;
            const auto k2         = k1 >> (ocp.lP - ocp.lvl);
            const auto v          = k2 % (1 << ocp.lvl);
            const auto t          = k1 - (k2 << (ocp.lP - ocp.lvl));
            return ((num_stages * t + i) << ocp.lvl) + v;
        };

        switch (settings.strategy) {
            case WarmStartingStrategy::Zeros:
                this->x0.reset();
                this->λ0.reset();
                this->y0.reset();
                break;
            case WarmStartingStrategy::Copy:
                this->x0 = x;
                this->λ0 = λ;
                this->y0 = y;
                break;
            case WarmStartingStrategy::Shift: {
                auto &x0 = this->x0.emplace(var_vec());         // TODO: zero init is redundant
                auto &λ0 = this->λ0.emplace(eq_constr_vec());   // TODO: zero init is redundant
                auto &y0 = this->y0.emplace(ineq_constr_vec()); // TODO: zero init is redundant
                for (index_t k = 1; k < ocp.N_horiz; ++k)       // TODO: vectorize?
                    x0(k_to_l(k - 1)) = x(k_to_l(k));
                x0(k_to_l(ocp.N_horiz - 1)) = x(k_to_l(ocp.N_horiz - 1));
                for (index_t k = 1; k < ocp.N_horiz; ++k) // TODO: vectorize?
                    y0(k_to_l(k - 1)) = y(k_to_l(k));
                y0(k_to_l(ocp.N_horiz - 1)) = y(k_to_l(ocp.N_horiz - 1));
                for (index_t k = 1; k < ocp.N_horiz; ++k) // TODO: vectorize?
                    λ0(k_to_l(k - 1)) = λ(k_to_l(k));
                λ0(k_to_l(ocp.N_horiz - 1)) = λ(k_to_l(ocp.N_horiz - 1));
            } break;
            case WarmStartingStrategy::ShiftNoInequality: {
                auto &x0 = this->x0.emplace(var_vec());       // TODO: zero init is redundant
                auto &λ0 = this->λ0.emplace(eq_constr_vec()); // TODO: zero init is redundant
                this->y0.emplace(y);
                for (index_t k = 1; k < ocp.N_horiz; ++k) // TODO: vectorize?
                    x0(k_to_l(k - 1)) = x(k_to_l(k));
                x0(k_to_l(ocp.N_horiz - 1)) = x(k_to_l(ocp.N_horiz - 1));
                for (index_t k = 1; k < ocp.N_horiz; ++k) // TODO: vectorize?
                    λ0(k_to_l(k - 1)) = λ(k_to_l(k));
                λ0(k_to_l(ocp.N_horiz - 1)) = λ(k_to_l(ocp.N_horiz - 1));
            } break;
            default: BATMAT_ASSERT(false);
        }
    }

    void reset() {
        num_updates         = 0;
        reset_factorization = true;
        if (ocp_timings)
            std::exchange(*ocp_timings, {});
    }

    [[nodiscard]] index_t num_var() const { return ocp.num_variables(); }
    [[nodiscard]] index_t num_eq_constr() const { return ocp.num_dynamics_constraints(); }
    [[nodiscard]] index_t num_ineq_constr() const { return ocp.num_general_constraints(); }

    [[nodiscard]] var_vec_t var_vec() const { return var_vec_t(ocp.initialize_variables()); }
    [[nodiscard]] eq_constr_vec_t eq_constr_vec() const {
        return eq_constr_vec_t(ocp.initialize_dynamics_constraints());
    }
    [[nodiscard]] ineq_constr_vec_t ineq_constr_vec() const {
        return ineq_constr_vec_t(ocp.initialize_general_constraints());
    }
    [[nodiscard]] active_set_t active_set() const {
        return active_set_t(ocp.initialize_general_constraints());
    }

    template <class... Js>
    void initialize_active_set(Js &...js) const {
        ([this](active_set_t &j) { j = active_set(); }(js), ...);
    }
    template <class... Xs>
    void initialize_var_vec(Xs &...xs) const {
        ([this](var_vec_t &x) { x = var_vec(); }(xs), ...);
    }
    template <class... Ys>
    void initialize_ineq_constr_vec(Ys &...ys) const {
        ([this](ineq_constr_vec_t &y) { y = ineq_constr_vec(); }(ys), ...);
    }
    template <class... Λs>
    void initialize_eq_constr_vec(Λs &...λs) const {
        ([this](eq_constr_vec_t &λ) { λ = eq_constr_vec(); }(λs), ...);
    }

    void initial_variables(Context &ctx, var_vec_t &x) const {
        if (x0) {
            xcopy(ctx, *x0, x);
            return;
        }
        set_constant(ctx, x, real_t{});
    }
    void initial_multipliers_eq(Context &ctx, eq_constr_vec_t &λ) const {
        if (λ0) {
            xcopy(ctx, *λ0, λ);
            return;
        }
        set_constant(ctx, λ, real_t{});
    }
    void initial_multipliers_ineq(Context &ctx, ineq_constr_vec_t &y) const {
        if (y0) {
            xcopy(ctx, *y0, y);
            return;
        }
        set_constant(ctx, y, real_t{});
    }

    // e = Ax - clamp(Ax, b_min, b_max)
    void ineq_constr_resid(Context &ctx, const ineq_constr_vec_t &Ax, ineq_constr_vec_t &e) const {
        auto t                   = get_timed(&OCP_t::Timings::ineq_constr_resid);
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            OCP_t::compact_blas::proj_diff(simdify(Ax.batch(di)), simdify(b_min_strided.batch(di)),
                                           simdify(b_max_strided.batch(di)), simdify(e.batch(di)));
        }
    }

    void project_multipliers_ineq(Context &ctx, ineq_constr_vec_t &y) const {
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            auto yi          = simdify(y.batch(di));
            auto b_min_i     = simdify(b_min_strided.batch(di));
            auto b_max_i     = simdify(b_max_strided.batch(di));
            for (index_t j = 0; j < yi.rows(); ++j) {
                auto yij            = batmat::datapar::aligned_load<simd>(&yi(0, j, 0));
                const auto b_min_ij = batmat::datapar::aligned_load<simd>(&b_min_i(0, j, 0)),
                           b_max_ij = batmat::datapar::aligned_load<simd>(&b_max_i(0, j, 0));
                // If upper bound is infinite, multiplier cannot be positive
                where(!isfinite(b_min_ij), yij) = fmax(yij, simd{0});
                where(!isfinite(b_max_ij), yij) = fmin(yij, simd{0});
                batmat::datapar::aligned_store(yij, &yi(0, j, 0));
            }
        }
    }

    real_t ineq_constr_viol(Context &ctx, const ineq_constr_vec_t &Ax) const {
        GUANAQO_TRACE("ineq_constr_viol", 0);
        auto t = get_timed(&OCP_t::Timings::ineq_constr_viol);
        using std::clamp;
        using std::isfinite;
        auto nrm_simd            = norms.zero_simd();
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            nrm_simd         = OCP_t::compact_blas::xreduce(
                nrm_simd,
                [](auto accum, auto Axi, auto b_min_i, auto b_max_i) {
                    auto zi = clamp(Axi, b_min_i, b_max_i);
                    return norms(accum, Axi - zi);
                },
                std::identity{}, simdify(Ax.batch(di))),
            simdify(b_min_strided.batch(di)), simdify(b_max_strided.batch(di));
        }
        auto nrm = ctx.reduce(norms(nrm_simd), norms.zero(), norms);
        return isfinite(nrm.asum) ? nrm.max : nrm.asum;
    }

    real_t ineq_constr_resid_al(Context &ctx, const ineq_constr_vec_t &y,
                                const ineq_constr_vec_t &ŷ, const ineq_constr_vec_t &Σ,
                                ineq_constr_vec_t &e) {
        GUANAQO_TRACE("ineq_constr_resid_al", 0);
        auto t = get_timed(&OCP_t::Timings::ineq_constr_resid_al);
        using std::clamp;
        using std::isfinite;
        auto nrm_simd            = norms.zero_simd();
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            nrm_simd         = OCP_t::compact_blas::xreduce_enumerate(
                nrm_simd,
                [di, &e](auto coord, auto accum, auto yi, auto ŷi, auto Σi) {
                    auto [i, r, c] = coord;
                    auto ei        = (ŷi - yi) / Σi;
                    datapar::aligned_store(ei, &e.batch(di)(i, r, c));
                    return norms(accum, ei);
                },
                std::identity{}, simdify(y.batch(di)), simdify(ŷ.batch(di)), simdify(Σ.batch(di)));
        }
        auto nrm = ctx.reduce(norms(nrm_simd), norms.zero(), norms);
        return isfinite(nrm.asum) ? nrm.max : nrm.asum;
    }

    void eq_constr_resid(Context &ctx, const var_vec_t &x, eq_constr_vec_t &Mxb) {
        ocp.residual_dynamics_constr(ctx, x, b_eq_strided, Mxb);
    }

    void mat_vec_MT(Context &ctx, const eq_constr_vec_t &λ, var_vec_t &Mᵀλ) {
        ocp.transposed_dynamics_constr(ctx, λ, Mᵀλ);
    }

    real_t unscaled_eq_constr_viol(Context &ctx, const eq_constr_vec_t &Mxb) const {
        return norm_inf(ctx, Mxb);
    }

    void mat_vec_AT(Context &ctx, const ineq_constr_vec_t &y, var_vec_t &Aᵀy) {
        ocp.transposed_general_constr(ctx, y, Aᵀy);
    }

    void mat_vec_AT(const ineq_constr_vec_t &y, var_vec_t &Aᵀy) {
        ocp.transposed_general_constr(y, Aᵀy);
    }

    void mat_vec_A(Context &ctx, const var_vec_t &x, ineq_constr_vec_t &Ax) {
        ocp.general_constr(ctx, x, Ax);
    }
    ineq_constr_vec_t mat_vec_A(Context &ctx, const var_vec_t &x) {
        auto Ax = ineq_constr_vec();
        mat_vec_A(ctx, x, Ax);
        return Ax;
    }

    void grad_f(Context &ctx, const var_vec_t &x, var_vec_t &grad_f) {
        ocp.cost_gradient(ctx, x, 1, grad_strided, 0, grad_f);
    }
    void grad_f_regularized(Context &ctx, real_t S, const var_vec_t &x, const var_vec_t &x_reg,
                            var_vec_t &grad_f) {
        using std::isfinite;
        if (isfinite(S))
            ocp.cost_gradient_regularized(ctx, S, x, x_reg, grad_strided, grad_f);
        else
            ocp.cost_gradient(ctx, x, 1, grad_strided, 0, grad_f);
    }
    void grad_f_remove_regularization(Context &ctx, real_t S, const var_vec_t &x,
                                      const var_vec_t &x_reg, var_vec_t &grad_f) {
        using std::isfinite;
        if (isfinite(S))
            ocp.cost_gradient_remove_regularization(ctx, S, x, x_reg, grad_f);
    }
    real_t f_grad_f(Context &ctx, const var_vec_t &x, var_vec_t &grad_f) {
        ocp.cost_gradient(ctx, x, 1, grad_strided, 0, grad_f);
        return std::numeric_limits<real_t>::quiet_NaN(); // TODO: compute f
    }
    std::tuple<real_t, var_vec_t> f_grad_f(Context &ctx, const var_vec_t &x) {
        auto grad_f = var_vec();
        real_t f    = f_grad_f(ctx, x, grad_f);
        return {f, std::move(grad_f)};
    }

    void update_regularization_changed(Context &ctx, real_t S_new, real_t S_old) {
        if (S_new != S_old) {
            ctx.arrive_and_wait(__LINE__);
            if (ctx.is_master())
                reset_factorization = true;
            ctx.arrive_and_wait(__LINE__);
        }
    }

    real_t boost_regularization(Context &ctx, real_t S, real_t S_boost) {
        update_regularization_changed(ctx, S_boost, S);
        return S_boost;
    }

    void update_penalty_changed(Context &ctx, const ineq_constr_vec_t &Σ, index_t num_Σ_changed) {
        std::ignore = Σ;
        if (num_Σ_changed > 0) {
            ctx.arrive_and_wait(__LINE__);
            if (ctx.is_master())
                reset_factorization = true;
            ctx.arrive_and_wait(__LINE__);
        }
    }

    template <class T, size_t N>
    void merge_chunk(std::span<const T> chunk, size_t chunk_index,
                     std::span<const std::array<size_t, N>> separators, std::span<T> out) {
        GUANAQO_TRACE("merge_chunk", 0, chunk.size());
        size_t num_chunks = separators.size();
        BATMAT_ASSUME(chunk_index < num_chunks);
        std::array<size_t, N> offsets{};
        for (size_t i = 0; i < N; ++i)
            for (size_t c = 0; c < chunk_index; ++c)
                offsets[i] += separators[c][i];
        for (size_t i = 0; i < N - 1; ++i)
            for (size_t c = chunk_index; c < num_chunks; ++c)
                offsets[i + 1] += separators[c][i];
        std::copy(chunk.begin(), chunk.begin() + separators[chunk_index][0],
                  out.begin() + offsets[0]);
        for (size_t i = 1; i < N; ++i)
            std::copy(chunk.begin() + separators[chunk_index][i - 1],
                      chunk.begin() + separators[chunk_index][i], out.begin() + offsets[i]);
    }

    BreakpointsResult
    compute_partition_breakpoints(Context &ctx, std::vector<Breakpoint> &breakpoints,
                                  const ineq_constr_vec_t &Σ, const ineq_constr_vec_t &y,
                                  const ineq_constr_vec_t &Ad, const ineq_constr_vec_t &Ax,
                                  const ineq_constr_vec_t &b_min, const ineq_constr_vec_t &b_max) {
        auto t = get_timed(&OCP_t::Timings::breakpoints);
        using std::isfinite;
        using std::sqrt;
        // Allocate memory
        const index_t ny_M       = std::max(ocp.ny, ocp.ny_0 + ocp.ny_N);
        const index_t m          = ocp.ceil_N * ny_M;
        const index_t P          = 1 << (ocp.lP - ocp.lvl);
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        if (ctx.is_master()) {
            breakpoints.resize(2 * m);
            breakpoints_temp.resize(2 * m);
            thread_indices.resize(P);
            thread_sums.resize(2 * P);
        }
        ctx.arrive_and_wait(__LINE__); // TODO: allocate ahead of time to avoid barrier
        // Parallelization and vectorization
        auto as = std::span{thread_sums}.first(P), bs = std::span{thread_sums}.subspan(P);
        auto thr_parts = std::span{thread_indices}.subspan(0, P);
        // Compute break points t[i] and intermediate values α[i] and δ[i]
        std::span<Breakpoint> neg_bp, pos_bp;
        const index_t ti        = ctx.index;
        Breakpoint *const fin_0 = breakpoints_temp.data() + 2 * ti * num_stages * ny_M * VL;
        Breakpoint *const inf_0 = fin_0 + 2 * num_stages * ny_M * VL;
        Breakpoint *fin = fin_0, *inf = inf_0;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = di0 + i;
            GUANAQO_TRACE("linesearch breakpoints cyqlone", di);
            for (index_t r = 0; r < ny_M; ++r) {
                const auto Σi  = batmat::datapar::aligned_load<simd>(&Σ.batch(di)(0, r, 0)),
                           yi  = batmat::datapar::aligned_load<simd>(&y.batch(di)(0, r, 0)),
                           Adi = batmat::datapar::aligned_load<simd>(&Ad.batch(di)(0, r, 0)),
                           Axi = batmat::datapar::aligned_load<simd>(&Ax.batch(di)(0, r, 0)),
                           li  = batmat::datapar::aligned_load<simd>(&b_min.batch(di)(0, r, 0)),
                           ui  = batmat::datapar::aligned_load<simd>(&b_max.batch(di)(0, r, 0));
                const auto s   = sqrt(Σi);
                const auto δ2 = s * Adi, δ1 = -δ2;
                const auto α1 = (yi + Σi * (Axi - li)) / s, α2 = (Σi * (ui - Axi) - yi) / s;
                const auto t1 = α1 / δ1, t2 = α2 / δ2;
                BATMAT_FULLY_UNROLLED_FOR (index_t v = 0; v < VL; ++v) {
                    *(isfinite(t1[v]) ? fin++ : --inf) = {.t = t1[v], .δ = δ1[v]};
                    *(isfinite(t2[v]) ? fin++ : --inf) = {.t = t2[v], .δ = δ2[v]};
                }
            }
        }
        // Partitioning the chunk of each thread separately improves partitioning performance
        // later on in the line search because of branch prediction.
        auto [pos, large] = [&] {
            GUANAQO_TRACE("linesearch breakpoints cyqlone partition", di0);
            auto pos   = partition(fin_0, fin, [](Breakpoint p) { return p.t <= 0; }).begin();
            auto large = partition(pos, fin, [](Breakpoint p) { return p.t <= 1; }).begin();
            return std::pair{pos, large};
        }();
        // Store the separator indices
        thr_parts[ti][0]    = pos - fin_0; // TODO: this is an all-to-all
        thr_parts[ti][1]    = large - fin_0;
        thr_parts[ti][2]    = fin - fin_0;
        thr_parts[ti][3]    = inf_0 - fin_0;
        auto thr_parts_done = ctx.arrive();
        // Compute the partial sums
        PartitionedBreakpoints pos_neg_bp{.neg_bp = std::span{fin_0, pos},
                                          .pos_bp = std::span{pos, fin}};
        auto ab = partial_sum_negative(pos_neg_bp);
        as[ti]  = ab.a; // We don't use an atomic accumulator here for reproducibility (float
        bs[ti]  = ab.b; // addition is not associative, and thread order is nondeterministic)
        // Synchronize the separator indices for all threads
        ctx.wait(std::move(thr_parts_done));
        auto as_bs_done = ctx.arrive();
        // Merge all local partitions of all threads into a single partitioned array
        GUANAQO_TRACE("linesearch breakpoints cyqlone merge", di0);
        merge_chunk<Breakpoint, 4>(std::span{fin_0, inf_0}, ti, thr_parts, std::span{breakpoints});
        ctx.wait(std::move(as_bs_done));
        auto merge_done = ctx.arrive();
        // Compute the final partition indices
        auto first_pos = std::accumulate(thr_parts.begin(), thr_parts.end(), breakpoints.begin(),
                                         [](auto it, auto &i) { return it += i[0]; }),
             first_inf = std::accumulate(thr_parts.begin(), thr_parts.end(), breakpoints.begin(),
                                         [](auto it, auto &i) { return it += i[2]; });
        neg_bp         = std::span{breakpoints.begin(), first_pos};
        pos_bp         = std::span{first_pos, first_inf};
        // Compute the final sums
        auto a = std::accumulate(begin(as), end(as), ABSum_t{}),
             b = std::accumulate(begin(bs), end(bs), ABSum_t{});
        // Wait for the full partitioning
        ctx.wait(std::move(merge_done));
        return {.bp = {.neg_bp = neg_bp, .pos_bp = pos_bp}, .ab_neg = {.a = a, .b = b}};
    }

    friend BreakpointsResult
    guanaqo_tag_invoke(guanaqo::tag_t<get_breakpoints>, CyqloneBackend &backend, Context &ctx,
                       std::vector<Breakpoint> &breakpoints, const ineq_constr_vec_t &Σ,
                       const ineq_constr_vec_t &y, const ineq_constr_vec_t &Ad,
                       const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &b_min,
                       const ineq_constr_vec_t &b_max) {
        return backend.compute_partition_breakpoints(ctx, breakpoints, Σ, y, Ad, Ax, b_min, b_max);
    }

    template <class T, class U>
    void xaxpy(Context &ctx, real_t a, const T &x, U &y) {
        const auto x_            = simdify(x);
        const auto y_            = simdify(y);
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            OCP_t::compact_blas::xaxpy(a, x_.batch(di), y_.batch(di));
        }
    }

    template <class T, class U>
    void xcopy(Context &ctx, const T &x, U &y) const {
        BATMAT_ASSERT(x.depth() == y.depth());
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            batmat::linalg::copy(x.batch(di), y.batch(di));
        }
    }

    template <class T, class U>
    void set_constant(Context &ctx, T &x, const U &y) const {
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            batmat::linalg::fill(y, x.batch(di));
        }
    }

    [[nodiscard]] real_t dot(Context &ctx, const var_vec_t &a, const var_vec_t &b) const {
        real_t sum               = 0;
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            sum += OCP_t::compact_blas::xdot(simdify(a.batch(di)), simdify(b.batch(di)));
        }
        return ctx.reduce(sum, real_t{});
    }

    template <class T>
    [[nodiscard]] auto norm_inf_l1_sq(Context &ctx, const T &x) const {
        auto nrm_simd            = norms.zero_simd();
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            nrm_simd         = OCP_t::compact_blas::xreduce(nrm_simd, norms, std::identity{},
                                                            simdify(x.batch(di)));
        }
        return ctx.reduce(norms(nrm_simd), norms.zero(), norms);
    }

    template <class T>
    [[nodiscard]] real_t norm_inf(Context &ctx, const T &x) const {
        using std::isfinite;
        auto nrm = norm_inf_l1_sq(ctx, x);
        return isfinite(nrm.asum) ? nrm.max : nrm.asum;
    }

    template <class T>
    [[nodiscard]] real_t norm_inf(const T &x) const {
        return OCP_t::compact_blas::xnrminf(simdify(x));
    }

    template <class T>
    [[nodiscard]] real_t norm_squared(Context &ctx, const T &x) const {
        real_t sum               = 0;
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            sum += OCP_t::compact_blas::xnrm2sq(simdify(x.batch(di)));
        }
        return ctx.reduce(sum, real_t{});
    }

    template <class T>
    void scale(Context &ctx, real_t s, T &x) const {
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            OCP_t::compact_blas::xaxpby(real_t{}, simdify(x.batch(di)), s, simdify(x.batch(di)));
        }
    }

    const ineq_constr_vec_t &Ax_min() const { return b_min_strided; }
    const ineq_constr_vec_t &Ax_max() const { return b_max_strided; }

    index_t calc_ŷ_Aᵀŷ(Context &ctx, const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &Σ,
                       const ineq_constr_vec_t &y, ineq_constr_vec_t &ŷ, var_vec_t &Aᵀŷ,
                       active_set_t &J) {
        using std::clamp;
        index_t count_J_local    = 0;
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        {
            auto t = get_timed(&OCP_t::Timings::calc_y_hat);
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t di = ti * num_stages + i;
                GUANAQO_TRACE("calc_ŷ_Aᵀŷ", di);
                for (index_t r = 0; r < y.rows(); ++r) {
                    const auto Σi  = batmat::datapar::aligned_load<simd>(&Σ.batch(di)(0, r, 0)),
                               yi  = batmat::datapar::aligned_load<simd>(&y.batch(di)(0, r, 0)),
                               Axi = batmat::datapar::aligned_load<simd>(&Ax.batch(di)(0, r, 0)),
                               li  = batmat::datapar::aligned_load<simd>(
                                   &b_min_strided.batch(di)(0, r, 0)),
                               ui = batmat::datapar::aligned_load<simd>(
                                   &b_max_strided.batch(di)(0, r, 0));
                    auto ζ  = Axi + yi / Σi;
                    auto z  = clamp(ζ, li, ui);
                    auto ŷi = yi + Σi * (Axi - z);
                    auto Ji = z != ζ; // TODO: inclusive?
                    datapar::aligned_store(ŷi, &ŷ.batch(di)(0, r, 0));
                    simd ΣJi{};
                    where(Ji, ΣJi) = Σi;
                    datapar::aligned_store(ΣJi, &J.batch(di)(0, r, 0));
                    count_J_local += static_cast<index_t>(popcount(Ji));
                }
            }
        }
        auto t = get_timed(&OCP_t::Timings::calc_y_hat_AT);
        mat_vec_AT(ctx, ŷ, Aᵀŷ);
        return ctx.reduce(count_J_local, index_t{});
    }

    real_t unscaled_aug_lagr_norm(Context &ctx, const var_vec_t &grad_f, const var_vec_t &Mᵀλ,
                                  const var_vec_t &Aᵀŷ) const {
        GUANAQO_TRACE("unscaled_aug_lagr_norm", 0);
        using std::clamp;
        using std::isfinite;
        auto nrm_simd            = norms.zero_simd();
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            nrm_simd         = OCP_t::compact_blas::xreduce(
                nrm_simd,
                [](auto accum, auto grad_fi, auto Mᵀλi, auto Aᵀŷi) {
                    auto grad_ali = grad_fi + Mᵀλi + Aᵀŷi;
                    return norms(accum, grad_ali);
                },
                std::identity{}, simdify(grad_f.batch(di)), simdify(Mᵀλ.batch(di)),
                simdify(Aᵀŷ.batch(di)));
        }
        auto nrm = ctx.reduce(norms(nrm_simd), norms.zero(), norms);
        return isfinite(nrm.asum) ? nrm.max : nrm.asum;
    }

    void scale_variables(std::span<const real_t> in, var_vec_t &out) const {
        ocp.pack_variables(in, out);
    }
    void scale_ineq_constr(std::span<const real_t> in, ineq_constr_vec_t &out) const {
        ocp.pack_constraints(in, out);
    }
    void scale_eq_constr(std::span<const real_t> in, eq_constr_vec_t &out) const {
        ocp.pack_dynamics(in, out);
    }

    void unscale_variables(const var_vec_t &in, std::span<real_t> out) const {
        ocp.unpack_variables(in, out);
    }
    void unscale_ineq_constr(const ineq_constr_vec_t &in, std::span<real_t> out) const {
        ocp.unpack_constraints(in, out);
    }
    void unscale_ineq_constr(const active_set_t &in, std::span<real_t> out) const {
        ocp.unpack_constraints(in, out);
    }
    void unscale_eq_constr(const eq_constr_vec_t &in, std::span<real_t> out) const {
        ocp.unpack_dynamics(in, out);
    }

    index_t active_set_change(Context &ctx, real_t, [[maybe_unused]] const ineq_constr_vec_t &Σ,
                              const active_set_t &J, const active_set_t &J_old) {
        BATMAT_ASSERT(J.rows() == J_old.rows() && J.cols() == J_old.cols());
        BATMAT_ASSERT(J.depth() == J_old.depth());
        BATMAT_ASSERT(J.cols() == 1);
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        index_t num_different    = 0;
        {
            GUANAQO_TRACE("active_set_change", 0);
            for (index_t i = 0; i < num_stages; ++i)
                num_different += [&] {
                    auto t           = get_timed(&OCP_t::Timings::update_active_set_change);
                    const index_t di = ti * num_stages + i;
                    auto Ji          = simdify(J.batch(di));
                    auto J_oldi      = simdify(J_old.batch(di));
                    return std::inner_product(Ji.data, Ji.data + Ji.size(), J_oldi.data, index_t{0},
                                              std::plus<>{}, std::not_equal_to<>{});
                }();
        }
        num_different = ctx.reduce(num_different, index_t{});
        // If there are no changing constraints, or if we were going to
        // re-factorize anyway, we don't need to do anything.
        if (num_different == 0 || reset_factorization)
            return num_different;
        bool do_reset_fac = num_updates >= settings.max_update_count;
        do_reset_fac |= static_cast<double>(num_different) >=
                        static_cast<double>(num_ineq_constr()) * settings.changing_constr_factor;
        if (do_reset_fac) {
            ctx.arrive_and_wait(__LINE__);
            if (ctx.is_master())
                reset_factorization = true;
            ctx.arrive_and_wait(__LINE__);
            return num_different;
        } else {
            ctx.arrive_and_wait(__LINE__);
            if (ctx.is_master()) {
                ++num_updates;
                ++stats.num_updates;
                stats.rank_updates += num_different;
            }
            ctx.arrive_and_wait(__LINE__);
        }
        // std::cout << "                                     -- Fact update\n";
        OCP_t::compact_blas::xsub_copy(simdify(ΔΣ), simdify(J), simdify(J_old));
        auto t = get_timed(&OCP_t::Timings::update_factorization);
        ocp.update(ctx, ΔΣ);
        return num_different;
    }

    void recompute_inner(Context &ctx, real_t S, const var_vec_t &x_outer, const var_vec_t &x,
                         const eq_constr_vec_t &λ, var_vec_t &grad, ineq_constr_vec_t &Ax,
                         var_vec_t &Mᵀλ) {
        {
            auto t = get_timed(&OCP_t::Timings::recompute_inner_grad);
            grad_f_regularized(ctx, S, x, x_outer, grad);
        }
        {
            auto t = get_timed(&OCP_t::Timings::recompute_inner_A);
            mat_vec_A(ctx, x, Ax);
        }
        {
            auto t = get_timed(&OCP_t::Timings::recompute_inner_MT);
            mat_vec_MT(ctx, λ, Mᵀλ);
        }
    }

    real_t recompute_outer(Context &ctx, const var_vec_t &x, const ineq_constr_vec_t &ŷ,
                           const eq_constr_vec_t &λ, var_vec_t &grad, ineq_constr_vec_t &Ax,
                           var_vec_t &Aᵀŷ, var_vec_t &Mᵀλ) {
        {
            auto t = get_timed(&OCP_t::Timings::recompute_outer_grad);
            grad_f(ctx, x, grad); // ∇f = Q * x + q
        }
        {
            auto t = get_timed(&OCP_t::Timings::recompute_outer_A);
            mat_vec_A(ctx, x, Ax); // Ax = A * x
        }
        {
            auto t = get_timed(&OCP_t::Timings::recompute_outer_AT);
            mat_vec_AT(ctx, ŷ, Aᵀŷ); // Aᵀŷ = Aᵀ * ŷ
        }
        {
            auto t = get_timed(&OCP_t::Timings::recompute_outer_MT);
            mat_vec_MT(ctx, λ, Mᵀλ); // Mᵀλ = Mᵀ * λ
        }
        {
            auto t = get_timed(&OCP_t::Timings::recompute_outer_norm);
            return unscaled_aug_lagr_norm(ctx, grad, Mᵀλ, Aᵀŷ);
        }
    }

    var_vec_t temp_var;
    eq_constr_vec_t temp_eq;
    ineq_constr_vec_t temp_ineq;

    void solve(Context &ctx, [[maybe_unused]] const var_vec_t &x, const var_vec_t &grad,
               const var_vec_t &Mᵀλ, const var_vec_t &Aᵀŷ, const eq_constr_vec_t &Mxb, real_t S,
               [[maybe_unused]] const ineq_constr_vec_t &Σ,
               const active_set_t &J, //
               var_vec_t &d, var_vec_t &ξ, ineq_constr_vec_t &Ad, eq_constr_vec_t &Δλ,
               var_vec_t &MᵀΔλ) {
        if (reset_factorization) {
            // std::cout << "                                     -- Fact reset\n";
            auto t = get_timed(&OCP_t::Timings::factor);
            ocp.factor(ctx, S, J, settings.factor_alt);
            ctx.arrive_and_wait(__LINE__);
            if (ctx.is_master()) {
                reset_factorization = false;
                num_updates         = 0;
                ++stats.num_factor;
            }
            ctx.arrive_and_wait(__LINE__);
        }
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        const index_t ti         = ctx.index;
        for (index_t i = 0; i < num_stages; ++i) {
            const index_t di = ti * num_stages + i;
            OCP_t::compact_blas::xadd_neg_copy(simdify(d.batch(di)), simdify(grad.batch(di)),
                                               simdify(Mᵀλ.batch(di)), simdify(Aᵀŷ.batch(di)));
            OCP_t::compact_blas::xadd_neg_copy(simdify(Δλ.batch(di)), simdify(Mxb.batch(di)));
        }
        if (settings.print_residuals) {
            int prec                      = settings.print_precision;
            auto grad_norm_inf            = norm_inf(ctx, d);
            auto grad_norm_sq             = norm_squared(ctx, d);
            auto constr_norm_inf          = norm_inf(ctx, Δλ);
            auto constr_norm_sq           = norm_squared(ctx, Δλ);
            auto cost_grad_norm_sq        = norm_squared(ctx, grad);
            auto eq_constr_grad_norm_sq   = norm_squared(ctx, Mᵀλ);
            auto ineq_constr_grad_norm_sq = norm_squared(ctx, Aᵀŷ);
            if (ctx.is_master()) {
                using std::sqrt;
                std::cout << "                   gradient:    abs∞="
                          << guanaqo::float_to_str(grad_norm_inf, prec)
                          << ",  abs₂=" << guanaqo::float_to_str(sqrt(grad_norm_sq), prec)
                          << "      {grad cost=" << guanaqo::float_to_str(sqrt(cost_grad_norm_sq))
                          << ",  Mᵀλ=" << guanaqo::float_to_str(sqrt(eq_constr_grad_norm_sq))
                          << ",  Aᵀŷ=" << guanaqo::float_to_str(sqrt(ineq_constr_grad_norm_sq))
                          << "}\n"
                          << "                constraints:    abs∞="
                          << guanaqo::float_to_str(constr_norm_inf, prec)
                          << ",  abs₂=" << guanaqo::float_to_str(sqrt(constr_norm_sq), prec)
                          << "\n";
            }
        }
        {
            auto t = get_timed(&OCP_t::Timings::solve);
            ocp.solve(ctx, d, Δλ);
        }
        {
            auto t = get_timed(&OCP_t::Timings::solve_MT);
            mat_vec_MT(ctx, Δλ, MᵀΔλ);
        }
        // Ad ← A d
        {
            auto t = get_timed(&OCP_t::Timings::solve_A);
            mat_vec_A(ctx, d, Ad);
        }
        // ξ ← Q d + S⁻¹ d
        {
            auto t = get_timed(&OCP_t::Timings::solve_grad);
            ocp.cost_gradient(ctx, d, 1 / S, d, 0, ξ);
        }

        if (settings.print_residuals) {
            if (ctx.is_master()) {
                temp_var  = var_vec();
                temp_eq   = eq_constr_vec();
                temp_ineq = ineq_constr_vec();
            }
            auto tm = get_timed(&OCP_t::Timings::solve_resid);
            ctx.arrive_and_wait(__LINE__);
            int prec = settings.print_precision;
            using std::abs;
            using std::isfinite;
            using std::max;
            using std::sqrt;
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t di = ti * num_stages + i;
                OCP_t::compact_blas::xhadamard(simdify(J.batch(di)), simdify(Ad.batch(di)),
                                               simdify(temp_ineq.batch(di)));
            }
            auto &res = temp_var;
            mat_vec_AT(ctx, temp_ineq, res);
            real_t r_norm_sq = 0, grad_norm_sq = 0, r_norm_inf = 0;
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t di = ti * num_stages + i;
                for (index_t r = 0; r < grad.rows(); ++r) {
                    auto gradi = datapar::aligned_load<simd>(&grad.batch(di)(0, r, 0)),
                         ξi    = datapar::aligned_load<simd>(&ξ.batch(di)(0, r, 0)),
                         Mᵀλi  = datapar::aligned_load<simd>(&Mᵀλ.batch(di)(0, r, 0)),
                         Aᵀŷi  = datapar::aligned_load<simd>(&Aᵀŷ.batch(di)(0, r, 0)),
                         MᵀΔλi = datapar::aligned_load<simd>(&MᵀΔλ.batch(di)(0, r, 0)),
                         ri    = datapar::aligned_load<simd>(&res.batch(di)(0, r, 0));
                    auto gi    = NeumaierSum(gradi) + Mᵀλi + Aᵀŷi;
                    ri += gi + MᵀΔλi + ξi;
                    datapar::aligned_store(ri, &res.batch(di)(0, r, 0));
                    r_norm_inf = max(r_norm_inf, hmax(abs(ri)));
                    r_norm_sq += reduce(ri * ri);
                    grad_norm_sq += reduce(simd{gi} * simd{gi});
                }
            }
            r_norm_sq    = ctx.reduce(r_norm_sq, real_t{});
            grad_norm_sq = ctx.reduce(grad_norm_sq, real_t{});
            r_norm_inf = ctx.reduce(r_norm_inf, real_t{}, [](auto a, auto b) { return max(a, b); });
            if (!isfinite(r_norm_sq))
                r_norm_inf = r_norm_sq;
            if (ctx.is_master())
                std::cout << "        RESID(stationarity):    abs∞="
                          << guanaqo::float_to_str(r_norm_inf, prec)
                          << ",  abs₂=" << guanaqo::float_to_str(sqrt(r_norm_sq), prec)
                          << ",  rel₂="
                          << guanaqo::float_to_str(sqrt(r_norm_sq / grad_norm_sq), prec) << "\n";
            auto &x_next = temp_var;
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t di = ti * num_stages + i;
                for (index_t r = 0; r < grad.rows(); ++r) {
                    auto xi  = datapar::aligned_load<simd>(&x.batch(di)(0, r, 0)),
                         dii = datapar::aligned_load<simd>(&d.batch(di)(0, r, 0));
                    datapar::aligned_store(xi + dii, &x_next.batch(di)(0, r, 0));
                }
            }
            auto &res_x_next = temp_eq;
            eq_constr_resid(ctx, x_next, res_x_next);
            real_t inf_res = norm_inf(ctx, res_x_next);
            if (ctx.is_master())
                std::cout << "        RESID(eq. feasibility): abs∞="
                          << guanaqo::float_to_str(inf_res, prec) << "\n";
        }
    }

    auto get_timed(typename OCP_t::Timings::type OCP_t::Timings::*member) const {
        return ocp_timings ? std::optional<typename OCP_t::Timings::timed_t>((*ocp_timings).*member)
                           : std::nullopt;
    }

    using Stats = CyqloneBackendStats;
    Stats stats = {};
    Stats clear_stats() { return std::exchange(stats, {}); }

    std::map<std::string, typename OCP_t::Timings::type> clear_timings() {
        if (!ocp_timings)
            return {};
        const auto t = std::exchange(*ocp_timings, {});
        return {
            {"breakpoints", t.breakpoints},
            {"calc_y_hat", t.calc_y_hat},
            {"calc_y_hat_AT", t.calc_y_hat_AT},
            {"update_active_set_change", t.update_active_set_change},
            {"update_factorization", t.update_factorization},
            {"factor", t.factor},
            {"solve", t.solve},
            {"solve_MT", t.solve_MT},
            {"solve_A", t.solve_A},
            {"solve_grad", t.solve_grad},
            {"solve_resid", t.solve_resid},
            {"recompute_outer_grad", t.recompute_outer_grad},
            {"recompute_outer_A", t.recompute_outer_A},
            {"recompute_outer_AT", t.recompute_outer_AT},
            {"recompute_outer_MT", t.recompute_outer_MT},
            {"recompute_outer_norm", t.recompute_outer_norm},
            {"recompute_inner_grad", t.recompute_inner_grad},
            {"recompute_inner_A", t.recompute_inner_A},
            {"recompute_inner_MT", t.recompute_inner_MT},
            {"ineq_constr_resid", t.ineq_constr_resid},
            {"ineq_constr_viol", t.ineq_constr_viol},
            {"ineq_constr_resid_al", t.ineq_constr_resid_al},
        };
    }
};

template <index_t VL, StorageOrder DefaultOrder>
unique_CyqloneBackend<VL, DefaultOrder>::~unique_CyqloneBackend() = default;

template <index_t VL, StorageOrder DefaultOrder>
unique_CyqloneBackend<VL, DefaultOrder>
make_qpalm_cyqlone_backend(const CyqloneStorage<> &ocp, CyqloneData data,
                           const CyqloneBackendSettings &settings) {
    return {std::make_unique<CyqloneBackend<VL, DefaultOrder>>(ocp, data, settings)};
}

template <index_t VL, StorageOrder DefaultOrder>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL, DefaultOrder> &backend,
                                  const CyqloneStorage<real_t> &ocp) {
    backend.update_data(ocp);
}

template <index_t VL, StorageOrder DefaultOrder>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL, DefaultOrder> &backend,
                                  const LinearOCPStorage &ocp) {
    const auto cocp = cyqlone::CyqloneStorage<>::build(ocp, backend.ocp.ny_0);
    update_qpalm_cyqlone_backend(backend, cocp);
}

} // namespace CYQLONE_NS(cyqlone::qpalm)
