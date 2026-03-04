#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>
#include <cyqlone/neumaier.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/implementation/breakpoint.hpp>
#include <cyqlone/reduce.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <batmat/linalg/copy.hpp>
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
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <tuple>
#include <utility>

namespace CYQLONE_NS(cyqlone::qpalm) {

namespace datapar = batmat::datapar;

template <index_t VL, StorageOrder DefaultOrder>
struct CyQPALMBackend {
    using OCP_t                 = cyqlone::CyqloneSolver<VL, real_t, DefaultOrder>;
    using Context               = typename OCP_t::Context;
    using storage_t             = typename OCP_t::template matrix<>;
    using simd                  = typename OCP_t::simd;
    static constexpr auto norms = cyqlone::norms<real_t, simd>{};
    // clang-format off
    struct var_vec_t         : storage_t { friend CyQPALMBackend; var_vec_t() = default;         private: var_vec_t(storage_t &&o)         : storage_t{std::move(o)} {} };
    struct eq_constr_vec_t   : storage_t { friend CyQPALMBackend; eq_constr_vec_t() = default;   private: eq_constr_vec_t(storage_t &&o)   : storage_t{std::move(o)} {} };
    struct ineq_constr_vec_t : storage_t { friend CyQPALMBackend; ineq_constr_vec_t() = default; private: ineq_constr_vec_t(storage_t &&o) : storage_t{std::move(o)} {} };
    struct active_set_t      : storage_t { friend CyQPALMBackend; active_set_t() = default;      private: active_set_t(storage_t &&o)      : storage_t{std::move(o)} {} };
    // clang-format on

    struct Timings {
        using type    = DefaultTimings;
        using timed_t = guanaqo::Timed<batmat::DefaultTimings>;
        type breakpoints{};
        type calc_y_hat{};
        type calc_y_hat_AT{};
        type update_active_set_change{};
        type update_factorization{};
        type factor{};
        type solve{};
        type solve_MT{};
        type solve_A{};
        type solve_grad{};
        type solve_resid{};
        type recompute_outer_grad{};
        type recompute_outer_A{};
        type recompute_outer_AT{};
        type recompute_outer_MT{};
        type recompute_outer_norm{};
        type recompute_inner_grad{};
        type recompute_inner_A{};
        type recompute_inner_MT{};
        type ineq_constr_resid{};
        type ineq_constr_viol{};
        type ineq_constr_resid_al{};
        type update_penalty_y{};
    };

    OCP_t ocp;
    std::unique_ptr<typename OCP_t::SharedContext> parallel_ctx = ocp.create_parallel_context();
    CyQPALMBackendSettings settings;
    ineq_constr_vec_t b_min_strided, b_max_strided;
    eq_constr_vec_t b_eq_strided;
    var_vec_t grad_strided;
    ineq_constr_vec_t ΔΣ;
    std::optional<var_vec_t> x0;
    std::optional<ineq_constr_vec_t> y0;
    std::optional<eq_constr_vec_t> λ0;
    std::vector<std::array<size_t, 4>> thread_indices;
    std::vector<Breakpoint> breakpoints_temp;

    bool reset_factorization = true;
    bool update_pending      = false;
    index_t num_updates      = 0;
    std::unique_ptr<Timings> ocp_timings;

    CyQPALMBackend(const CyqloneStorage<> &ocp, CyqloneData data,
                   const CyQPALMBackendSettings &settings)
        : ocp{OCP_t::build(ocp, settings.processors)}, settings{settings} {
        this->parallel_ctx->barrier.spin_count = settings.spin_count;
        this->ocp.update_tricyqle_params(settings.tricyqle_params);
        b_min_strided = ineq_constr_vec();
        b_max_strided = ineq_constr_vec();
        ΔΣ            = ineq_constr_vec();
        b_eq_strided  = eq_constr_vec();
        grad_strided  = var_vec();
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
            ocp_timings = std::make_unique<Timings>();
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
        // TODO: this does not handle the case N_horiz != ceil_N() correctly
        const auto k_to_l = [&](index_t k) {
            const auto num_stages = ocp.n;
            const auto i          = (ocp.ceil_N() - k) % num_stages;
            const auto k1         = (k + i) / num_stages;
            const auto k2         = k1 >> ocp.lp();
            const auto v          = k2 % (1 << ocp.lv());
            const auto t          = k1 - (k2 << ocp.lp());
            return ((num_stages * t + i) << ocp.lv()) + v;
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
        update_pending      = false;
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
        x0 ? xcopy(ctx, *x0, x) : set_constant(ctx, x, real_t{});
    }
    void initial_multipliers_eq(Context &ctx, eq_constr_vec_t &λ) const {
        λ0 ? xcopy(ctx, *λ0, λ) : set_constant(ctx, λ, real_t{});
    }
    void initial_multipliers_ineq(Context &ctx, ineq_constr_vec_t &y) const {
        y0 ? xcopy(ctx, *y0, y) : set_constant(ctx, y, real_t{});
    }

    // e = Ax - clamp(Ax, b_min, b_max)
    void ineq_constr_resid(Context &ctx, const ineq_constr_vec_t &Ax, ineq_constr_vec_t &e) const;

    void project_multipliers_ineq(Context &ctx, ineq_constr_vec_t &y) const;

    real_t ineq_constr_viol(Context &ctx, const ineq_constr_vec_t &Ax) const;

    real_t ineq_constr_resid_al(Context &ctx, const ineq_constr_vec_t &y,
                                const ineq_constr_vec_t &ŷ, const ineq_constr_vec_t &Σ,
                                ineq_constr_vec_t &e);

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

    struct PenaltySettings {
        real_t θ;
        real_t Δy, Δy_always;
        real_t max_penalty_y;
    };

    /// Increase the penalty parameters Σ for which the violation e has not decreased sufficiently
    /// compared to e_old.
    index_t update_penalty_y(Context &ctx, ineq_constr_vec_t &Σ, const ineq_constr_vec_t &e,
                             const ineq_constr_vec_t &e_old, const PenaltySettings &settings);

    /// Called when the primal regularization S has changed: causes the factorization to be reset.
    void update_regularization_changed(Context &ctx, real_t S_new, real_t S_old) {
        if (S_new != S_old)
            ctx.run_single_sync([&] { reset_factorization = true; });
    }

    /// Decrease the primal regularization S to S_boost, to boost convergence when close to the
    /// solution.
    real_t boost_regularization(Context &ctx, real_t S, real_t S_boost) {
        update_regularization_changed(ctx, S_boost, S);
        return S_boost;
    }

    /// Called when the penalty parameters Σ have been updated: causes the factorization to be reset
    /// if any of the penalty parameters have changed.
    void update_penalty_changed(Context &ctx, const ineq_constr_vec_t &Σ, index_t num_Σ_changed) {
        std::ignore = Σ;
        if (num_Σ_changed > 0)
            ctx.run_single_sync([&] { reset_factorization = true; });
    }

    /// @name Line search
    /// @{

    template <class T, size_t N>
    static void merge_chunk(std::span<const T> chunk, size_t chunk_index,
                            std::span<const std::array<size_t, N>> separators, std::span<T> out);

    BreakpointsResult
    compute_partition_breakpoints(Context &ctx, std::vector<Breakpoint> &breakpoints,
                                  const ineq_constr_vec_t &Σ, const ineq_constr_vec_t &y,
                                  const ineq_constr_vec_t &Ad, const ineq_constr_vec_t &Ax,
                                  const ineq_constr_vec_t &b_min, const ineq_constr_vec_t &b_max);

    friend BreakpointsResult
    guanaqo_tag_invoke(guanaqo::tag_t<get_breakpoints>, CyQPALMBackend &backend, Context &ctx,
                       std::vector<Breakpoint> &breakpoints, const ineq_constr_vec_t &Σ,
                       const ineq_constr_vec_t &y, const ineq_constr_vec_t &Ad,
                       const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &b_min,
                       const ineq_constr_vec_t &b_max) {
        return backend.compute_partition_breakpoints(ctx, breakpoints, Σ, y, Ad, Ax, b_min, b_max);
    }

    /// @}

    /// @name Linear algebra operations (level 1 BLAS-like)
    /// @{

    /// Compute y = a x + y.
    template <class T, class U>
    void xaxpy(Context &ctx, real_t a, const T &x, U &y);
    /// Copy x to y.
    template <class T, class U>
    void xcopy(Context &ctx, const T &x, U &y) const;
    /// Set each element of x to the constant value y.
    template <class T, class U>
    void set_constant(Context &ctx, T &x, const U &y) const;
    /// Multiply a vector x by a scalar s.
    template <class T>
    void scale(Context &ctx, real_t s, T &x) const;
    /// Dot product of a and b.
    [[nodiscard]] real_t dot(Context &ctx, const var_vec_t &a, const var_vec_t &b) const;
    /// Compute multiple partial dot products, without reducing across threads.
    template <class... Args>
    void local_dots(std::span<real_t, 1 + sizeof...(Args) / 2> out, const auto &a, const auto &b,
                    const Args &...others) const;
    /// Compute multiple dot products at once. This is more efficient than computing them separately
    /// because only a single reduction across threads is needed.
    template <class... Args>
    [[nodiscard]] std::array<real_t, sizeof...(Args) / 2> dots(Context &ctx,
                                                               const Args &...args) const;
    /// Compute the infinity, l1 and l2 norms of x.
    template <class T>
    [[nodiscard]] auto norm_inf_l1_sq(Context &ctx, const T &x) const;
    /// Infinity or max norm of x.
    template <class T>
    [[nodiscard]] real_t norm_inf(Context &ctx, const T &x) const;
    /// Squared l2 norm of x.
    template <class T>
    [[nodiscard]] real_t norm_squared(Context &ctx, const T &x) const;

    /// @}

    const ineq_constr_vec_t &Ax_min() const { return b_min_strided; }
    const ineq_constr_vec_t &Ax_max() const { return b_max_strided; }

    index_t calc_ŷ_Aᵀŷ(Context &ctx, const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &Σ,
                       const ineq_constr_vec_t &y, ineq_constr_vec_t &ŷ, var_vec_t &Aᵀŷ,
                       active_set_t &J);

    real_t unscaled_aug_lagr_norm(Context &ctx, const var_vec_t &grad_f, const var_vec_t &Mᵀλ,
                                  const var_vec_t &Aᵀŷ) const {
        GUANAQO_TRACE("unscaled_aug_lagr_norm", 0);
        auto nrm_simd             = norms.zero_simd();
        const auto grad_norm_simd = [&](auto grad_fji, auto Mᵀλji, auto Aᵀŷji) {
            nrm_simd = norms(nrm_simd, grad_fji + Mᵀλji + Aᵀŷji);
        };
        const auto grad_norm_batch = [&](auto, auto, auto grad_fj, auto Mᵀλj, auto Aᵀŷj) {
            linalg::for_each_elementwise(grad_norm_simd, grad_fj, Mᵀλj, Aᵀŷj);
        };
        ocp.foreach_stage(ctx, grad_norm_batch, grad_f, Mᵀλ, Aᵀŷ);
        return ctx.reduce(norms(nrm_simd), norms).norm_inf();
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
        index_t num_different        = 0;
        const auto active_set_change = [&](auto, auto, auto Ji, auto J_oldi) {
            num_different += std::inner_product(Ji.data(), Ji.data() + Ji.size(), J_oldi.data(), //
                                                index_t{0}, std::plus<>{}, std::not_equal_to<>{});
        };
        {
            GUANAQO_TRACE("active_set_change", 0);
            auto t = get_timed(&Timings::update_active_set_change);
            ocp.foreach_stage(ctx, active_set_change, J, J_old);
        }
        num_different = ctx.reduce(num_different);
        // If there are no changing constraints, or if we were going to
        // re-factorize anyway, we don't need to do anything.
        if (num_different == 0 || reset_factorization)
            return num_different;
        bool do_reset_fac = num_updates >= settings.max_update_count;
        do_reset_fac |= static_cast<double>(num_different) >=
                        static_cast<double>(num_ineq_constr()) * settings.changing_constr_factor;
        bool prev_update_pending = update_pending;
        if (do_reset_fac) {
            ctx.run_single_sync([&] { reset_factorization = true; });
            return num_different;
        }
        ctx.run_single_sync([&] {
            update_pending = true;
            ++num_updates;
            ++stats.num_updates;
            stats.rank_updates += num_different;
        });
        auto t = get_timed(&Timings::update_factorization);
        if (prev_update_pending) {
            const auto delta = [&](auto, auto, auto Ji, auto J_oldi, auto ΔΣi) {
                linalg::axpy(ΔΣi, {1, -1}, Ji, J_oldi);
            };
            ocp.foreach_stage(ctx, delta, J, J_old, ΔΣ);
        } else {
            const auto delta = [&](auto, auto, auto Ji, auto J_oldi, auto ΔΣi) {
                linalg::sub(Ji, J_oldi, ΔΣi);
            };
            ocp.foreach_stage(ctx, delta, J, J_old, ΔΣ);
        }
        return num_different;
    }

    void recompute_inner(Context &ctx, real_t S, const var_vec_t &x_outer, const var_vec_t &x,
                         const eq_constr_vec_t &λ, var_vec_t &grad, ineq_constr_vec_t &Ax,
                         var_vec_t &Mᵀλ) {
        {
            auto t = get_timed(&Timings::recompute_inner_grad);
            grad_f_regularized(ctx, S, x, x_outer, grad);
        }
        {
            auto t = get_timed(&Timings::recompute_inner_A);
            mat_vec_A(ctx, x, Ax);
        }
        {
            auto t = get_timed(&Timings::recompute_inner_MT);
            mat_vec_MT(ctx, λ, Mᵀλ);
        }
    }

    real_t recompute_outer(Context &ctx, const var_vec_t &x, const var_vec_t &Aᵀŷ,
                           const eq_constr_vec_t &λ, var_vec_t &grad, ineq_constr_vec_t &Ax,
                           var_vec_t &Mᵀλ) {
        {
            auto t = get_timed(&Timings::recompute_outer_grad);
            grad_f(ctx, x, grad); // ∇f = Q * x + q
        }
        {
            auto t = get_timed(&Timings::recompute_outer_A);
            mat_vec_A(ctx, x, Ax); // Ax = A * x
        }
        {
            auto t = get_timed(&Timings::recompute_outer_MT);
            mat_vec_MT(ctx, λ, Mᵀλ); // Mᵀλ = Mᵀ * λ
        }
        {
            auto t = get_timed(&Timings::recompute_outer_norm);
            return unscaled_aug_lagr_norm(ctx, grad, Mᵀλ, Aᵀŷ);
        }
    }

    var_vec_t temp_var;
    eq_constr_vec_t temp_eq;
    ineq_constr_vec_t temp_ineq;

    void print_solve_rhs_norms(Context &ctx, const var_vec_t &d, const eq_constr_vec_t &Δλ,
                               const var_vec_t &grad, const var_vec_t &Mᵀλ,
                               const var_vec_t &Aᵀŷ) const;

    void print_solve_resid_norms(Context &ctx, const var_vec_t &x, const var_vec_t &d,
                                 const var_vec_t &grad, const var_vec_t &ξ, const var_vec_t &Mᵀλ,
                                 const var_vec_t &Aᵀŷ, const var_vec_t &MᵀΔλ,
                                 const ineq_constr_vec_t &Ad, const active_set_t &J);
    void solve(Context &ctx, [[maybe_unused]] const var_vec_t &x, const var_vec_t &grad,
               const var_vec_t &Mᵀλ, const var_vec_t &Aᵀŷ, const eq_constr_vec_t &Mxb, real_t S,
               [[maybe_unused]] const ineq_constr_vec_t &Σ,
               const active_set_t &J, //
               var_vec_t &d, var_vec_t &ξ, ineq_constr_vec_t &Ad, eq_constr_vec_t &Δλ,
               var_vec_t &MᵀΔλ);

    auto get_timed(Timings::type Timings::*member) const {
        return ocp_timings ? std::optional<typename Timings::timed_t>((*ocp_timings).*member)
                           : std::nullopt;
    }

    using Stats = CyQPALMBackendStats;
    Stats stats = {};
    Stats clear_stats() { return std::exchange(stats, {}); }

    std::map<std::string, typename Timings::type> clear_timings() {
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
            {"update_penalty_y", t.update_penalty_y},
        };
    }
};

template <index_t VL, StorageOrder DefaultOrder>
unique_CyQPALMBackend<VL, DefaultOrder>::~unique_CyQPALMBackend() = default;

template <index_t VL, StorageOrder DefaultOrder>
unique_CyQPALMBackend<VL, DefaultOrder>
make_cyqpalm_backend(const CyqloneStorage<> &ocp, CyqloneData data,
                     const CyQPALMBackendSettings &settings) {
    return {std::make_unique<CyQPALMBackend<VL, DefaultOrder>>(ocp, data, settings)};
}

template <index_t VL, StorageOrder DefaultOrder>
void update_cyqpalm_backend(CyQPALMBackend<VL, DefaultOrder> &backend,
                            const CyqloneStorage<real_t> &ocp) {
    backend.update_data(ocp);
}

template <index_t VL, StorageOrder DefaultOrder>
void update_cyqpalm_backend(CyQPALMBackend<VL, DefaultOrder> &backend,
                            const LinearOCPStorage &ocp) {
    const auto cocp = cyqlone::CyqloneStorage<>::build(ocp, backend.ocp.ny_0);
    update_cyqpalm_backend(backend, cocp);
}

} // namespace CYQLONE_NS(cyqlone::qpalm)

#include <cyqlone/qpalm/backends/backend-cyqlone/ineq-constr.tpp>
#include <cyqlone/qpalm/backends/backend-cyqlone/linalg.tpp>
#include <cyqlone/qpalm/backends/backend-cyqlone/linesearch.tpp>
#include <cyqlone/qpalm/backends/backend-cyqlone/solve.tpp>
