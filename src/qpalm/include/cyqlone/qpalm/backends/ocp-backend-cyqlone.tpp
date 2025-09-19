#pragma once

#include <cyqlone/cyqlone.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <batmat/assume.hpp>
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
#include <ranges>
#include <string>
#include <tuple>
#include <utility>

namespace cyqlone::qpalm {

using batmat::linalg::simdify;
namespace datapar = batmat::datapar;

template <index_t VL>
struct CyqloneBackend {
    using OCP_t          = cyqlone::CyqloneSolver<VL>;
    using storage_t      = typename OCP_t::template matrix<>;
    using mask_storage_t = typename OCP_t::template mask_matrix<>;
    using simd           = typename OCP_t::compact_blas::simd;
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

    bool reset_factorization = true;
    index_t num_updates      = 0;
    std::unique_ptr<typename OCP_t::Timings> ocp_timings;

    CyqloneBackend(const CyqloneStorage<> &ocp, CyqloneData data,
                   const CyqloneBackendSettings &settings)
        : ocp{OCP_t::build(ocp, settings.log_processors)}, settings{settings} {
        this->ocp.alt                      = settings.factor_alt;
        this->ocp.pcg_max_iter             = settings.pcg_max_iter;
        this->ocp.pcg_tolerance            = settings.pcg_tolerance;
        this->ocp.pcg_print_resid          = settings.pcg_print_resid;
        this->ocp.use_stair_preconditioner = settings.use_stair_preconditioner;
        b_min_strided                      = ineq_constr_vec();
        b_max_strided                      = ineq_constr_vec();
        ΔΣ                                 = ineq_constr_vec();
        b_eq_strided                       = eq_constr_vec();
        grad_strided                       = var_vec();
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
                for (index_t i = 1; i < x0.depth(); ++i)        // TODO: vectorize?
                    x0(i - 1) = x(i);
                x0(x0.depth() - 1) = x(x0.depth() - 1);
                for (index_t i = 1; i < y0.depth(); ++i) // TODO: vectorize?
                    y0(i - 1) = y(i);
                y0(y0.depth() - 1) = y(y0.depth() - 1);
                for (index_t i = 1; i < λ0.depth(); ++i) // TODO: vectorize?
                    λ0(i - 1) = λ(i);
                λ0(λ0.depth() - 1) = λ(λ0.depth() - 1);
            } break;
            case WarmStartingStrategy::ShiftNoInequality: {
                auto &x0 = this->x0.emplace(var_vec());       // TODO: zero init is redundant
                auto &λ0 = this->λ0.emplace(eq_constr_vec()); // TODO: zero init is redundant
                this->y0.emplace(y);
                for (index_t i = 1; i < x0.depth(); ++i) // TODO: vectorize?
                    x0(i - 1) = x(i);
                x0(x0.depth() - 1) = x(x0.depth() - 1);
                for (index_t i = 1; i < λ0.depth(); ++i) // TODO: vectorize?
                    λ0(i - 1) = λ(i);
                λ0(λ0.depth() - 1) = λ(λ0.depth() - 1);
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

    void initial_variables(var_vec_t &x) const {
        if (x0) {
            x.view() = x0->view();
            return;
        }
        set_constant(x, real_t{});
    }
    void initial_multipliers_eq(eq_constr_vec_t &λ) const {
        if (λ0) {
            λ.view() = λ0->view();
            return;
        }
        set_constant(λ, real_t{});
    }
    void initial_multipliers_ineq(ineq_constr_vec_t &y) const {
        if (y0) {
            y.view() = y0->view();
            return;
        }
        set_constant(y, real_t{});
    }

    // e = Ax - clamp(Ax, b_min, b_max)
    void ineq_constr_resid(const ineq_constr_vec_t &Ax, ineq_constr_vec_t &e) const {
        for (index_t i = 0; i < Ax.num_batches(); ++i)
            OCP_t::compact_blas::proj_diff(simdify(Ax.batch(i)), simdify(b_min_strided.batch(i)),
                                           simdify(b_max_strided.batch(i)), simdify(e.batch(i)));
    }

    // e = Ax - clamp(Ax, b_min, b_max)
    ineq_constr_vec_t ineq_constr_resid(const ineq_constr_vec_t &Ax) const {
        auto e = ineq_constr_vec();
        ineq_constr_resid(Ax, e);
        return e;
    }

    static auto inf_norm_accumulate(auto accum, auto t) {
        using std::abs;
        using std::max;
        auto at = abs(t);
        return std::array{max(at, accum[0]), at + accum[1]};
    }

    static std::array<real_t, 2> inf_norm_reduce(std::array<simd, 2> accum) {
        return {hmax(accum[0]), reduce(accum[1])};
    }

    real_t ineq_constr_viol(const ineq_constr_vec_t &Ax) const {
        GUANAQO_TRACE("ineq_constr_viol", 0);
        using std::clamp;
        using std::isfinite;
        auto [inf_nrm, l1_norm] = OCP_t::compact_blas::xreduce(
            std::array<simd, 2>{0, 0},
            [](auto accum, auto Axi, auto b_min_i, auto b_max_i) {
                auto zi = clamp(Axi, b_min_i, b_max_i);
                return inf_norm_accumulate(accum, Axi - zi);
            },
            inf_norm_reduce, Ax.view(), b_min_strided.view(), b_max_strided.view());
        return isfinite(l1_norm) ? inf_nrm : l1_norm;
    }

    real_t ineq_constr_resid_al(const ineq_constr_vec_t &y, const ineq_constr_vec_t &ŷ,
                                const ineq_constr_vec_t &Σ, ineq_constr_vec_t &e) {
        GUANAQO_TRACE("ineq_constr_resid_al", 0);
        using std::clamp;
        using std::isfinite;
        auto [inf_nrm, l1_norm] = OCP_t::compact_blas::xreduce_enumerate(
            std::array<simd, 2>{0, 0},
            [&e](auto coord, auto accum, auto yi, auto ŷi, auto Σi) {
                auto [i, r, c] = coord;
                auto ei        = (ŷi - yi) / Σi;
                if constexpr (std::is_same_v<decltype(ei), simd>)
                    datapar::aligned_store(ei, &e(i, r, c));
                else
                    e(i, r, c) = ei;
                return inf_norm_accumulate(accum, ei);
            },
            inf_norm_reduce, simdify(y), simdify(ŷ), simdify(Σ));
        return isfinite(l1_norm) ? inf_nrm : l1_norm;
    }

    void eq_constr_resid(const var_vec_t &x, eq_constr_vec_t &Mxb) {
        ocp.residual_dynamics_constr(x, b_eq_strided, Mxb);
    }
    eq_constr_vec_t eq_constr_resid(const var_vec_t &x) {
        auto Mxb = eq_constr_vec();
        eq_constr_resid(x, Mxb);
        return Mxb;
    }

    void mat_vec_MT(const eq_constr_vec_t &λ, var_vec_t &Mᵀλ) {
        ocp.transposed_dynamics_constr(λ, Mᵀλ);
    }
    var_vec_t mat_vec_MT(const eq_constr_vec_t &λ) {
        auto Mᵀλ = var_vec();
        mat_vec_MT(λ, Mᵀλ);
        return Mᵀλ;
    }

    real_t unscaled_eq_constr_viol(const eq_constr_vec_t &Mxb) const { return norm_inf(Mxb); }

    void mat_vec_AT(const ineq_constr_vec_t &y, var_vec_t &Aᵀy) {
        ocp.transposed_general_constr(y, Aᵀy);
    }

    void mat_vec_A(const var_vec_t &x, ineq_constr_vec_t &Ax) { ocp.general_constr(x, Ax); }
    ineq_constr_vec_t mat_vec_A(const var_vec_t &x) {
        auto Ax = ineq_constr_vec();
        mat_vec_A(x, Ax);
        return Ax;
    }

    void grad_f(const var_vec_t &x, var_vec_t &grad_f) {
        ocp.cost_gradient(x, 1, grad_strided, 0, grad_f);
    }
    void grad_f_regularized(real_t S, const var_vec_t &x, const var_vec_t &x_reg,
                            var_vec_t &grad_f) {
        using std::isfinite;
        if (isfinite(S))
            ocp.cost_gradient_regularized(S, x, x_reg, grad_strided, grad_f);
        else
            ocp.cost_gradient(x, 1, grad_strided, 0, grad_f);
    }
    void grad_f_remove_regularization(real_t S, const var_vec_t &x, const var_vec_t &x_reg,
                                      var_vec_t &grad_f) {
        using std::isfinite;
        if (isfinite(S))
            ocp.cost_gradient_remove_regularization(S, x, x_reg, grad_f);
    }
    real_t f_grad_f(const var_vec_t &x, var_vec_t &grad_f) {
        ocp.cost_gradient(x, 1, grad_strided, 0, grad_f);
        return std::numeric_limits<real_t>::quiet_NaN(); // TODO: compute f
    }
    std::tuple<real_t, var_vec_t> f_grad_f(const var_vec_t &x) {
        auto grad_f = var_vec();
        real_t f    = f_grad_f(x, grad_f);
        return {f, std::move(grad_f)};
    }

    void update_regularization_changed(real_t S_new, real_t S_old) {
        if (S_new != S_old)
            reset_factorization = true;
    }

    real_t boost_regularization(real_t S, real_t S_boost) {
        update_regularization_changed(S_boost, S);
        return S_boost;
    }

    void update_penalty_changed(const ineq_constr_vec_t &Σ,
                                std::span<const real_t> Σ_update_factors,
                                std::span<const index_t> constr_changed) {
        std::ignore = Σ;
        std::ignore = Σ_update_factors;
        if (!constr_changed.empty())
            reset_factorization = true;
    }

    template <class Breakpoint>
    std::span<Breakpoint>
    compute_breakpoints(std::vector<Breakpoint> &breakpoints, const ineq_constr_vec_t &Σ,
                        const ineq_constr_vec_t &y, const ineq_constr_vec_t &Ad,
                        const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &b_min,
                        const ineq_constr_vec_t &b_max) const {
        GUANAQO_TRACE("linesearch breakpoints cyqlone", 0);
        using std::sqrt;
        // Allocate memory
        const auto ny_M = std::max(ocp.ny, ocp.ny_0 + ocp.ny_N);
        const auto m    = ocp.ceil_N * ny_M;
        breakpoints.resize(2 * m);
        // Parallelization and vectorization
        const index_t P          = 1 << (ocp.lP - ocp.lvl);
        const index_t num_stages = ocp.ceil_N >> ocp.lP; // number of stages per thread
        using simd               = batmat::datapar::deduced_simd<real_t, VL>;
        // Compute break points t[i] and intermediate values α[i] and δ[i]
        batmat::foreach_thread(P, [&](index_t ti, index_t) {
            const index_t di0 = ti * num_stages;
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t di = di0 + i;
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
                        const index_t l = (di * ny_M + r) * VL + v;
                        BATMAT_ASSUME(l < m);
                        breakpoints[l]     = {.t = t1[v], .δ = δ1[v], .α = α1[v]};
                        breakpoints[m + l] = {.t = t2[v], .δ = δ2[v], .α = α2[v]};
                    }
                }
            }
        });
        return std::span{breakpoints};
    }

    template <class T, class U>
    static void xaxpy(real_t a, const T &x, U &y) {
        OCP_t::compact_blas::xaxpy(a, simdify(x), simdify(y));
    }

    template <class T, class U>
    static void xcopy(const T &x, U &y) {
        BATMAT_ASSERT(x.depth() == y.depth());
        for (index_t l = 0; l < x.num_batches(); ++l)
            batmat::linalg::copy(x.batch(l), y.batch(l));
    }

    template <class T, class U>
    static void set_constant(T &x, const U &y) {
        for (index_t l = 0; l < x.num_batches(); ++l)
            batmat::linalg::fill(y, x.batch(l));
    }

    [[nodiscard]] real_t dot(const var_vec_t &a, const var_vec_t &b) const {
        return OCP_t::compact_blas::xdot(simdify(a), simdify(b));
    }

    [[nodiscard]] real_t norm_inf(const ineq_constr_vec_t &x) const {
        return OCP_t::compact_blas::xnrminf(simdify(x));
    }

    [[nodiscard]] static real_t norm_inf(const eq_constr_vec_t &x) {
        return OCP_t::compact_blas::xnrminf(simdify(x));
    }

    [[nodiscard]] real_t norm_squared(const ineq_constr_vec_t &x) const {
        return OCP_t::compact_blas::xnrm2sq(simdify(x));
    }
    [[nodiscard]] real_t norm_squared(const var_vec_t &x) const {
        return OCP_t::compact_blas::xnrm2sq(simdify(x));
    }
    void scale(real_t s, var_vec_t &x) const {
        return OCP_t::compact_blas::xaxpby(real_t{}, simdify(x), s, simdify(x));
    }
    void scale(real_t s, eq_constr_vec_t &x) const {
        return OCP_t::compact_blas::xaxpby(real_t{}, simdify(x), s, simdify(x));
    }
    void scale(real_t s, ineq_constr_vec_t &x) const {
        return OCP_t::compact_blas::xaxpby(real_t{}, simdify(x), s, simdify(x));
    }

    const ineq_constr_vec_t &Ax_min() const { return b_min_strided; }
    const ineq_constr_vec_t &Ax_max() const { return b_max_strided; }

    index_t calc_ŷ_Aᵀŷ(const ineq_constr_vec_t &Ax, const ineq_constr_vec_t &Σ,
                       const ineq_constr_vec_t &y, ineq_constr_vec_t &ŷ, var_vec_t &Aᵀŷ,
                       active_set_t &J) {
        using std::clamp;
        auto count_J = [&] {
            GUANAQO_TRACE("calc_ŷ_Aᵀŷ", 0);
            return OCP_t::compact_blas::xreduce_enumerate(
                index_t{0},
                [&ŷ, &J](auto coord, auto accum, auto Axi, auto Σi, auto yi, auto li, auto ui) {
                    auto [i, r, c] = coord;
                    auto ζ         = Axi + yi / Σi;
                    auto z         = clamp(ζ, li, ui);
                    auto ŷi        = yi + Σi * (Axi - z);
                    auto Ji        = z != ζ; // TODO: inclusive?
                    if constexpr (std::is_same_v<decltype(ŷi), simd>) {
                        datapar::aligned_store(ŷi, &ŷ(i, r, c));
                        simd ΣJi{};
                        where(Ji, ΣJi) = Σi;
                        datapar::aligned_store(ΣJi, &J(i, r, c));
                        return static_cast<index_t>(popcount(Ji)) + accum;
                    } else {
                        ŷ(i, r, c) = ŷi;
                        J(i, r, c) = Ji ? Σi : 0;
                        return static_cast<index_t>(Ji) + accum;
                    }
                },
                std::identity{}, simdify(Ax), simdify(Σ), simdify(y), simdify(b_min_strided),
                simdify(b_max_strided));
        }();
        mat_vec_AT(ŷ, Aᵀŷ);
        return count_J;
    }

    real_t unscaled_aug_lagr_norm(const var_vec_t &grad_f, const var_vec_t &Mᵀλ,
                                  const var_vec_t &Aᵀŷ) {
        GUANAQO_TRACE("unscaled_aug_lagr_norm", 0);
        using std::clamp;
        using std::isfinite;
        auto [inf_nrm, l1_norm] = OCP_t::compact_blas::xreduce(
            std::array<simd, 2>{0, 0},
            [](auto accum, auto grad_fi, auto Mᵀλi, auto Aᵀŷi) {
                auto grad_ali = grad_fi + Mᵀλi + Aᵀŷi;
                return inf_norm_accumulate(accum, grad_ali);
            },
            inf_norm_reduce, simdify(grad_f), simdify(Mᵀλ), simdify(Aᵀŷ));
        return isfinite(l1_norm) ? inf_nrm : l1_norm;
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

    index_t active_set_change(real_t, [[maybe_unused]] const ineq_constr_vec_t &Σ,
                              const active_set_t &J, const active_set_t &J_old) {
        assert(std::ranges::size(J) == std::ranges::size(J_old));
        BATMAT_ASSERT(J.view().layer_stride() == J.rows());
        BATMAT_ASSERT(J.outer_stride() == J.rows());
        BATMAT_ASSERT(J.cols() == 1);
        auto size_J = std::max(ocp.ny, ocp.ny_0 + ocp.ny_N) * ocp.ceil_N;
        BATMAT_ASSERT(size_J == std::ranges::ssize(J));
        auto num_different = [&] {
            GUANAQO_TRACE("active_set_change", 0);
            return std::inner_product(J.data(), J.data() + size_J, J_old.data(), index_t{0},
                                      std::plus<>{}, std::not_equal_to<>{});
        }();
        // If there are no changing constraints, or if we were going to
        // re-factorize anyway, we don't need to do anything.
        if (num_different == 0 || reset_factorization)
            return num_different;
        bool do_reset_fac = ++num_updates > settings.max_update_count;
        do_reset_fac |= static_cast<double>(num_different) >=
                        static_cast<double>(num_ineq_constr()) * settings.changing_constr_factor;
        if (do_reset_fac) {
            reset_factorization = true;
            return num_different;
        }
        // std::cout << "                                     -- Fact update\n";
        OCP_t::compact_blas::xsub_copy(simdify(ΔΣ), simdify(J), simdify(J_old));
        ocp.update(ΔΣ);
        return num_different;
    }

    void recompute_inner(real_t S, const var_vec_t &x_outer, const var_vec_t &x,
                         const eq_constr_vec_t &λ, var_vec_t &grad, ineq_constr_vec_t &Ax,
                         var_vec_t &Mᵀλ) {
        grad_f_regularized(S, x, x_outer, grad);
        mat_vec_A(x, Ax);
        mat_vec_MT(λ, Mᵀλ);
    }

    real_t recompute_outer(const var_vec_t &x, const ineq_constr_vec_t &ŷ, const eq_constr_vec_t &λ,
                           var_vec_t &grad, ineq_constr_vec_t &Ax, var_vec_t &Aᵀŷ, var_vec_t &Mᵀλ) {
        grad_f(x, grad);    // ∇f = Q * x + q
        mat_vec_A(x, Ax);   // Ax = A * x
        mat_vec_AT(ŷ, Aᵀŷ); // Aᵀŷ = Aᵀ * ŷ
        mat_vec_MT(λ, Mᵀλ); // Mᵀλ = Mᵀ * λ
        auto stationarity = unscaled_aug_lagr_norm(grad, Mᵀλ, Aᵀŷ);
        return stationarity;
    }

    void solve([[maybe_unused]] const var_vec_t &x, const var_vec_t &grad, const var_vec_t &Mᵀλ,
               const var_vec_t &Aᵀŷ, const eq_constr_vec_t &Mxb, real_t S,
               [[maybe_unused]] const ineq_constr_vec_t &Σ,
               const active_set_t &J, //
               var_vec_t &d, var_vec_t &ξ, ineq_constr_vec_t &Ad, eq_constr_vec_t &Δλ,
               var_vec_t &MᵀΔλ) {
        if (std::exchange(reset_factorization, false)) {
            // std::cout << "                                     -- Fact reset\n";
            ocp.factor(S, J, settings.factor_alt);
            num_updates = 0;
        }
        OCP_t::compact_blas::xadd_neg_copy(simdify(d), simdify(grad), simdify(Mᵀλ), simdify(Aᵀŷ));
        OCP_t::compact_blas::xadd_neg_copy(simdify(Δλ), simdify(Mxb));
        if (settings.print_residuals) {
            int prec             = settings.print_precision;
            auto grad_norm_inf   = OCP_t::compact_blas::xnrminf(simdify(d));
            auto grad_norm_sq    = OCP_t::compact_blas::xnrm2sq(simdify(d));
            auto constr_norm_inf = OCP_t::compact_blas::xnrminf(simdify(Δλ));
            auto constr_norm_sq  = OCP_t::compact_blas::xnrm2sq(simdify(Δλ));
            std::cout << "                   gradient:    abs∞="
                      << guanaqo::float_to_str(grad_norm_inf, prec)
                      << ",  abs₂=" << guanaqo::float_to_str(sqrt(grad_norm_sq), prec) << "\n"
                      << "                constraints:    abs∞="
                      << guanaqo::float_to_str(constr_norm_inf, prec)
                      << ",  abs₂=" << guanaqo::float_to_str(sqrt(constr_norm_sq), prec) << "\n";
        }
        ocp.solve(d, Δλ);
        mat_vec_MT(Δλ, MᵀΔλ);
        // Ad ← A d
        mat_vec_A(d, Ad);
        // ξ ← Q d + S⁻¹ d
        ocp.cost_gradient(d, 1 / S, d, 0, ξ);

        if (settings.print_residuals) {
            int prec = settings.print_precision;
            using std::abs;
            using std::max;
            using std::sqrt;
            using std::views::zip;
            auto t = ineq_constr_vec();
            for (auto &&[ti, Ji, Adi] : zip(t, J, Ad))
                ti = Ji * Adi;
            auto r = var_vec();
            mat_vec_AT(t, r);
            real_t r_norm_sq = 0, grad_norm_sq = 0, r_norm_inf = 0;
            for (auto &&[gradi, Mᵀλi, Aᵀŷi, MᵀΔλi, ξi, ri] : zip(grad, Mᵀλ, Aᵀŷ, MᵀΔλ, ξ, r)) {
                real_t gi = gradi + Mᵀλi + Aᵀŷi;
                ri += gi + MᵀΔλi + ξi;
                r_norm_inf = max(r_norm_inf, abs(ri));
                r_norm_sq += ri * ri;
                grad_norm_sq += gi * gi;
            }
            std::cout << "        RESID(stationarity):    abs∞="
                      << guanaqo::float_to_str(r_norm_inf, prec)
                      << ",  abs₂=" << guanaqo::float_to_str(sqrt(r_norm_sq), prec)
                      << ",  rel₂=" << guanaqo::float_to_str(sqrt(r_norm_sq / grad_norm_sq), prec)
                      << "\n";
            auto x_next = var_vec();
            for (auto &&[x_nexti, xi, di] : zip(x_next, x, d))
                x_nexti = xi + di;
            auto res_x_next = eq_constr_resid(x_next);
            real_t inf_res  = 0;
            for (auto res_x_nexti : res_x_next)
                inf_res = std::max(inf_res, std::abs(res_x_nexti));
            std::cout << "        RESID(eq. feasibility): abs∞="
                      << guanaqo::float_to_str(inf_res, prec) << "\n";
        }
    }

    std::map<std::string, batmat::DefaultTimings> clear_timings() {
        if (!ocp_timings)
            return {};
        [[maybe_unused]] auto t = std::exchange(*ocp_timings, {});
        return {
            // TODO
        };
    }
};

template <index_t VL>
unique_CyqloneBackend<VL>::~unique_CyqloneBackend() = default;

template <index_t VL>
unique_CyqloneBackend<VL> make_qpalm_cyqlone_backend(const CyqloneStorage<> &ocp, CyqloneData data,
                                                     const CyqloneBackendSettings &settings) {
    return {std::make_unique<CyqloneBackend<VL>>(ocp, data, settings)};
}

template <index_t VL>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL> &backend, const CyqloneStorage<real_t> &ocp) {
    backend.update_data(ocp);
}

template <index_t VL>
void update_qpalm_cyqlone_backend(CyqloneBackend<VL> &backend, const LinearOCPStorage &ocp,
                                  std::span<const real_t> qr, std::span<const real_t> b_eq,
                                  std::span<const real_t> b_lb, std::span<const real_t> b_ub) {
    const auto cocp = cyqlone::CyqloneStorage<>::build(ocp, qr, b_eq, b_lb, b_ub, backend.ocp.ny_0);
    update_qpalm_cyqlone_backend(backend, cocp);
}

} // namespace cyqlone::qpalm
