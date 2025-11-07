#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/qpalm/detailed-stats.hpp>
#include <cyqlone/qpalm/implementation/linesearch.tpp>
#include <cyqlone/qpalm/solver.hpp>
#include <guanaqo/atomic-stop-signal.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/timed.hpp>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <utility>

namespace CYQLONE_NS(cyqlone::qpalm) {

template <class Backend>
struct SolverImplementation {
    using backend_type = Backend;
    static_assert(!std::is_const_v<backend_type>);

    using active_set_t = typename backend_type::active_set_t;
    using ineq_vec_t   = typename backend_type::ineq_constr_vec_t;
    using eq_vec_t     = typename backend_type::eq_constr_vec_t;
    using var_vec_t    = typename backend_type::var_vec_t;
    LineSearch<ineq_vec_t> linesearch;

    static void initialize_penalty_y(Backend::Context &ctx, backend_type &backend, real_t f0,
                                     const ineq_vec_t &e0, ineq_vec_t &Σ,
                                     const Settings &settings) {
        using std::abs;
        using std::fmax;
        using std::fmin;
        auto numer = fmax(1, abs(f0));
        auto denom = fmax(1, 0.5 * backend.norm_squared(ctx, e0));
        auto Σ0    = settings.initial_penalty_y;
        if (settings.scale_initial_penalty_y)
            Σ0 = fmax(1e-4, fmin(Σ0 * numer / denom, 1e4));
        backend.set_constant(ctx, Σ, Σ0);
    }

    static index_t update_penalty_y(backend_type &backend, ineq_vec_t &Σ, const ineq_vec_t &e,
                                    const ineq_vec_t &e_old, const Settings &settings) {
        GUANAQO_TRACE("update_penalty_y", 0);
        using std::abs;
        using std::fmax;
        using std::fmin;
        using std::views::zip;
        const real_t min_denom  = 1e-6;
        const real_t norm_inf_e = fmax(min_denom, backend.norm_inf(e));
        index_t num_changed     = 0;
        for (auto &&[ei, ei_old, Σi] : zip(e, e_old, Σ)) {
            bool insufficient_progress = abs(ei) > settings.θ * abs(ei_old);
            real_t update_factor = insufficient_progress ? settings.Δy * abs(ei) / norm_inf_e : 1;
            update_factor *= settings.Δy_always;
            real_t Σ_new = Σi * update_factor;
            Σ_new        = fmax(Σi * settings.Δy_always, fmin(Σ_new, settings.max_penalty_y));
            num_changed += Σ_new != Σi;
            Σi = Σ_new;
        }
        return num_changed;
    }

    static index_t update_penalty_y(Backend::Context &ctx, backend_type &backend, ineq_vec_t &Σ,
                                    const ineq_vec_t &e, const ineq_vec_t &e_old,
                                    const Settings &settings) {
        ctx.arrive_and_wait();
        index_t num_changed = 0;
        if (ctx.is_master())
            num_changed = update_penalty_y(backend, Σ, e, e_old, settings);
        return ctx.broadcast(num_changed);
    }

    static real_t update_penalty_x(real_t S, const Settings &settings) {
        using std::fmin;
        return fmin(settings.Δx * S, settings.max_penalty_x);
    }

    void ensure_storage(backend_type &backend) {
        backend.reset();
        backend.initialize_active_set(active_set, active_set_old);
        backend.initialize_ineq_constr_vec(Σ, y, ŷ, e, e_old, Ax, Ad);
        backend.initialize_eq_constr_vec(Mxb, Δλ, λ);
        backend.initialize_var_vec(x, grad, Mᵀλ, Aᵀŷ, x_outer, MᵀΔλ, d, ξ, grad_add);
    }

    SolverStatus do_main_loop(Backend::Context &ctx, backend_type &backend,
                              const Settings &settings, guanaqo::AtomicStopSignal &stop_signal,
                              SolverStats &stats);

    active_set_t active_set, active_set_old;
    ineq_vec_t Σ, y, ŷ, e, e_old, Ax, Ad;
    eq_vec_t Mxb, Δλ, λ;
    var_vec_t x, grad, Mᵀλ, Aᵀŷ, x_outer, MᵀΔλ, d, ξ, grad_add;
    // TODO: we don't really need grad_add unless we need to compute the directional derivative
};

template <class Backend>
SolverStatus Solver<Backend>::do_solve() {
    assert(backend);
    if (!impl)
        impl = std::make_unique<SolverImplementation<backend_type>>();
    impl->ensure_storage(*backend);
    SolverStatus status;
#if GUANAQO_WITH_TRACING
    backend->ocp.parallel_ctx->run([](auto &ctx) { GUANAQO_TRACE("thread_id", ctx.index); });
#endif
    backend->ocp.parallel_ctx->run([&](backend_type::Context &ctx) {
        SolverStats stats;
        auto status_local = impl->do_main_loop(ctx, *backend, settings, stop_signal, stats);
        if (ctx.is_master()) {
            status      = status_local;
            this->stats = std::move(stats);
        }
        GUANAQO_TRACE("end", ctx.index);
    });
    return status;
}

template <class Backend>
SolverStatus SolverImplementation<Backend>::do_main_loop(Backend::Context &ctx,
                                                         backend_type &backend,
                                                         const Settings &settings,
                                                         guanaqo::AtomicStopSignal &stop_signal,
                                                         SolverStats &stats) {
    using clock_t = std::chrono::steady_clock;
    using guanaqo::float_to_str;
    using std::abs;
    using std::cbrt;
    using std::isfinite;
    using std::sqrt;
    using std::swap;
    clock_t::time_point start_time = clock_t::now();
    real_t inner_tol               = settings.initial_inner_tolerance;
    real_t ineq_constr_resid       = std::numeric_limits<real_t>::quiet_NaN();
    unsigned inner_iter = 0, outer_iter = 0;
    SolverTimings timings{};
    std::optional<DetailedStats> detailed_stats{};
    if (settings.detailed_stats)
        detailed_stats.emplace();

    // Initial guess
    backend.initial_variables(ctx, x);
    backend.initial_multipliers_eq(ctx, λ);
    backend.initial_multipliers_ineq(ctx, y);

    real_t S = settings.initial_penalty_x;
    // Thread-local views for swapping
    auto y = std::ref(this->y), ŷ = std::ref(this->ŷ);
    auto active_set = std::ref(this->active_set), active_set_old = std::ref(this->active_set_old);
    auto e = std::ref(this->e), e_old = std::ref(this->e_old);

    // Initialize matrix-vector products and initial penalty
    timed(timings.mat_vec_M, [&] {
        backend.eq_constr_resid(ctx, x, Mxb); // Mxb = M * x - b
    });
    timed(timings.mat_vec_A, [&] {
        backend.mat_vec_A(ctx, x, Ax); // Ax = A * x
    });
    backend.ineq_constr_resid(ctx, Ax, e); // e = Ax - clamp(Ax, b_min, b_max)
    real_t f0 = timed(timings.mat_vec_Q, [&] {
        return backend.f_grad_f(ctx, x, grad); // ∇f = Q * x + q
    });
    timed(timings.mat_vec_MT, [&] {
        backend.mat_vec_MT(ctx, λ, Mᵀλ); // Mᵀλ = Mᵀ * λ
    });

    // Initial penalties
    initialize_penalty_y(ctx, backend, f0, e, Σ, settings);
    std::ignore = f0; // TODO

    // Outer ALM loop
    while (true) {
        backend.xcopy(ctx, x, x_outer);

        // Inner semismooth Newton loop
        unsigned no_change_active_set = 0;
        bool force_τ_1_active_set     = false;
        auto remaining_iter           = settings.max_total_inner_iter - inner_iter;
        remaining_iter                = std::min(remaining_iter, settings.max_inner_iter);
        real_t stationarity           = std::numeric_limits<real_t>::infinity();
        real_t eq_resid               = std::numeric_limits<real_t>::infinity();
        bool increase_penalty_y       = true;
        for (unsigned inner = 0; true; ++inner) {
            // Compute gradient of augmented Lagrangian
            index_t nJ = timed(timings.mat_vec_AT, [&] {
                return backend.calc_ŷ_Aᵀŷ(ctx, Ax, Σ, y, ŷ, Aᵀŷ, active_set);
            });

            // What to do upon inner loop termination
            auto leave_inner = [&] {
                // Remove the primal regularization from the gradient because
                // we'll replace x_outer by x
                backend.grad_f_remove_regularization(ctx, S, x, x_outer, grad);
                GUANAQO_TRACE("leave_inner", inner);
                // x contains x_next, ŷ contains y_next, Aᵀŷ contains Aᵀy_next
                inner_iter += inner;
            };

            // Check inner loop termination
            bool first_iter  = outer_iter == 0 && inner == 0;
            bool check_eq    = first_iter || settings.recompute_eq_res;
            eq_resid         = check_eq ? backend.unscaled_eq_constr_viol(ctx, Mxb) : 0;
            real_t eq_tol    = settings.eq_constr_tolerance;
            stationarity     = backend.unscaled_aug_lagr_norm(ctx, grad, Mᵀλ, Aᵀŷ);
            bool out_of_iter = inner >= remaining_iter;
            bool out_of_time = clock_t::now() >= start_time + settings.max_time;
            bool inf_err     = !isfinite(stationarity + eq_resid);
            bool stop        = stop_signal.stop_requested();
            bool fail        = out_of_iter || out_of_time || inf_err || stop;
            bool inner_conv  = stationarity <= inner_tol && eq_resid <= eq_tol;

            if (detailed_stats) {
                const auto nan = std::numeric_limits<real_t>::quiet_NaN();
                detailed_stats->entries.push_back({
                    .outer_iter                  = outer_iter,
                    .inner_iter                  = inner,
                    .stationarity                = stationarity,
                    .ineq_constr_viol            = ineq_constr_resid,
                    .eq_constr_viol              = eq_resid,
                    .linesearch_step_size        = nan,
                    .linesearch_breakpoint_index = 0,
                    .num_active_constr           = nJ,
                    .num_changing_constr         = 0,
                    .exit_reason                 = DetailedStats::ExitReason::Busy,
                });
            }

            if (inner_conv || fail) {
                increase_penalty_y &= !fail;
                if (detailed_stats)
                    detailed_stats->entries.back().exit_reason =
                        inner_conv ? DetailedStats::ExitReason::Converged
                                   : DetailedStats::ExitReason::Fail;
                if (settings.verbose && ctx.is_master()) {
                    int prec           = settings.print_precision;
                    const char *status = inner_conv ? "\x1b[0;32mConverged\x1b[0m" /* green */
                                                    : "\x1b[0;31mFail\x1b[0m" /* red */;
                    std::cout << "    Exit inner: " << status << ": #J = " << std::setw(6) << nJ
                              << ", stationarity=" << float_to_str(stationarity, prec)
                              << ", eq constr resid=" << float_to_str(eq_resid, prec) << "\n\n";
                }
                leave_inner();
                break;
            }

            // Check if the active set changed
            auto active_set_change = timed(timings.active_set_change, [&] {
                return backend.active_set_change(ctx, S, Σ, active_set, active_set_old);
            });
            swap(active_set, active_set_old);
            if (detailed_stats)
                detailed_stats->entries.back().num_changing_constr = active_set_change;
            if (!active_set_change &&
                ++no_change_active_set >= settings.max_no_changes_active_set) {
                if (force_τ_1_active_set || !settings.force_linesearch_if_no_set_change) {
                    increase_penalty_y = false;
                    if (detailed_stats)
                        detailed_stats->entries.back().exit_reason =
                            DetailedStats::ExitReason::NoActiveSetChange;
                    if (settings.verbose && ctx.is_master())
                        std::cout << "    Exit inner: \x1b[0;32mNo active set "
                                     "change\x1b[0m\n\n";
                    leave_inner();
                    break;
                }
                force_τ_1_active_set = true;
            } else {
                force_τ_1_active_set = false;
            }

            // Update regularization
            bool upd_reg_iter = inner == 0 && outer_iter > 0;
            if (upd_reg_iter && ineq_constr_resid <= settings.dual_tolerance)
                if (!active_set_change)
                    timed(timings.boost_regularization, [&] {
                        S = backend.boost_regularization(ctx, S, settings.boost_penalty_x);
                    });

            // Solve the Newton system
            timed(timings.solve, [&] {
                backend.solve(ctx, x, grad, Mᵀλ, Aᵀŷ, Mxb, S, Σ, active_set_old, //
                              d, ξ, Ad, Δλ, MᵀΔλ);
            });
            real_t scal_d = 1;
            if (settings.scale_newton_step) {
                scal_d = 1 / sqrt(backend.norm_squared(ctx, d));
                backend.scale(ctx, scal_d, d);
                backend.scale(ctx, scal_d, ξ);
                backend.scale(ctx, scal_d, Ad);
                backend.scale(ctx, scal_d, Δλ);
                backend.scale(ctx, scal_d, MᵀΔλ);
            }
            bool force_τ_1_dir_deriv = false;
            if (settings.print_directional_deriv || settings.force_linesearch_if_dir_deriv_pos ||
                settings.detailed_stats) {
                backend.xcopy(ctx, grad, grad_add);
                backend.xaxpy(ctx, 1, Aᵀŷ, grad_add);
                real_t dir_deriv = backend.dot(ctx, d, grad_add);
                if (settings.print_directional_deriv && ctx.is_master()) {
                    const char *color = dir_deriv < 0 ? "\x1b[0;33m" /* green */
                                                      : "\x1b[0;31m" /* red */;
                    std::cout << "dir deriv: " << color << dir_deriv << "\x1b[0m" << std::endl;
                }
                if (settings.force_linesearch_if_dir_deriv_pos && dir_deriv > 0)
                    force_τ_1_dir_deriv = true;
            }

            // Perform exact line search
            bool force_τ_1_first_iter = (inner_iter + inner) == 0;
            real_t τ                  = 1 / scal_d;
            index_t iτ                = -999999;
            if (force_τ_1_active_set || force_τ_1_dir_deriv || force_τ_1_first_iter) {
                if (settings.verbose && !force_τ_1_first_iter && ctx.is_master())
                    std::cout << "    \x1b[0;33mWarning\x1b[0m: Forcing line "
                                 "search τ=1\n";
            } else {
                std::tie(τ, iτ) = timed(timings.line_search, [&] {
                    real_t η = backend.dot(ctx, d, ξ), β = backend.dot(ctx, d, grad);
                    if (settings.linesearch_include_multipliers) {
                        real_t dMᵀΔλ = backend.dot(ctx, d, MᵀΔλ), dMᵀλ = backend.dot(ctx, d, Mᵀλ);
                        if (settings.print_linesearch_inputs && ctx.is_master()) {
                            std::cout << "                η = " << η << "\n"
                                      << "        <d, MᵀΔλ> = " << dMᵀΔλ << "\n"
                                      << "                β = " << β << "\n"
                                      << "        <d, Mᵀλ> = " << dMᵀλ << "\n";
                        }
                        η += dMᵀΔλ;
                        β += dMᵀλ;
                    }
                    return linesearch(ctx, backend, η, β, Σ, y, Ad, Ax, backend.Ax_min(),
                                      backend.Ax_max());
                });
            }

            if (detailed_stats) {
                detailed_stats->entries.back().linesearch_step_size        = τ;
                detailed_stats->entries.back().linesearch_breakpoint_index = iτ;
            }

            const real_t τ_min = 1e-8 / scal_d, τ_max = 1e2 / scal_d;
            if (settings.verbose && ctx.is_master()) {
                int prec          = settings.print_precision;
                const auto eps    = cbrt(std::numeric_limits<real_t>::epsilon());
                const char *color = abs(1 - τ) < eps ? "\x1b[0;32m" /* green */
                                    : τ > τ_max      ? "\x1b[0;35m" /* pink */
                                    : τ > τ_min      ? "\x1b[0;33m" /* yellow */
                                                     : "\x1b[0;31m" /* red */;
                std::cout << "    inner " << std::setw(4) << inner << " (" << std::setw(4)
                          << (inner_iter + inner) << "): #J = " << std::setw(6) << nJ
                          << ", #ΔJ = " << std::setw(6) << active_set_change
                          << ", stationarity=" << float_to_str(stationarity, prec)
                          << ", eq constr resid=" << float_to_str(eq_resid, prec) << ", τ=" << color
                          << float_to_str(τ) << "\x1b[0m (" << iτ << ")\n";
            }
            τ = std::clamp(τ, τ_min, τ_max);

            { // Apply step
                GUANAQO_TRACE("apply step", inner);
                backend.xaxpy(ctx, τ, d, x);
                backend.xaxpy(ctx, τ, Δλ, λ);
            }

            // Optionally recompute Ax and ∇f
            if (settings.recompute_inner) {
                timed(timings.recompute_inner,
                      [&] { backend.recompute_inner(ctx, S, x_outer, x, λ, grad, Ax, Mᵀλ); });
            } else {
                GUANAQO_TRACE("apply step derived", inner);
                backend.xaxpy(ctx, τ, Ad, Ax);
                backend.xaxpy(ctx, τ, MᵀΔλ, Mᵀλ);
                backend.xaxpy(ctx, τ, ξ, grad);
            }

            // Compute new equality constraint residual
            if (settings.recompute_eq_res)
                timed(timings.mat_vec_M, [&] {
                    backend.eq_constr_resid(ctx, x, Mxb); //
                });
            else
                backend.set_constant(ctx, Mxb, real_t{});
        }
        ++outer_iter;

        // Compute constraint violation
        swap(e, e_old);
        ineq_constr_resid = backend.ineq_constr_resid_al(ctx, y, ŷ, Σ, e);

        if (settings.recompute) {
            stationarity = timed(timings.recompute_outer, [&] {
                return backend.recompute_outer(ctx, x, ŷ, λ, grad, Ax, Aᵀŷ, Mᵀλ);
            });
        }

        // Print progress
        if (settings.verbose) {
            auto nrm_Σ = backend.norm_inf(ctx, Σ);
            if (ctx.is_master()) {
                int prec = settings.print_precision;
                std::cout << "outer " << std::setw(4) << outer_iter
                          << ": stationarity=" << float_to_str(stationarity, prec)
                          << ", constraints=" << float_to_str(ineq_constr_resid, prec)
                          << ", penalty=" << float_to_str(nrm_Σ, prec)
                          << ", regularization=" << float_to_str(1 / S, prec) << '\n'
                          << std::endl;
            }
        }

        // Check stopping criteria
        bool converged = stationarity <= settings.tolerance &&
                         ineq_constr_resid <= settings.dual_tolerance &&
                         (!settings.recompute_eq_res || eq_resid <= settings.eq_constr_tolerance);
        bool out_of_iter =
            inner_iter >= settings.max_total_inner_iter || outer_iter >= settings.max_outer_iter;
        bool out_of_time = clock_t::now() >= start_time + settings.max_time;
        bool inf_err     = !std::isfinite(stationarity + ineq_constr_resid);
        bool stop        = stop_signal.stop_requested();
        // Return solution
        if (converged || out_of_iter || out_of_time || inf_err || stop) {
            stats.inner_iter           = inner_iter;
            stats.outer_iter           = outer_iter;
            stats.detail               = std::move(detailed_stats);
            stats.stationarity         = stationarity;
            stats.primal_residual_norm = ineq_constr_resid;
            stats.max_penalty          = backend.norm_inf(ctx, Σ);
            stats.timings              = std::move(timings);
            if (ctx.is_master())
                stats.timings.backend = backend.clear_timings();
            swap(y, ŷ);
            if (&this->y != &y.get())
                backend.xcopy(ctx, y.get(), this->y);
            return converged     ? SolverStatus::Converged
                   : out_of_iter ? SolverStatus::MaxIter
                   : out_of_time ? SolverStatus::MaxTime
                   : inf_err     ? SolverStatus::NotFinite
                   : stop        ? SolverStatus::Interrupted
                                 : SolverStatus::Busy;
        }

        // Update penalty factors
        if (increase_penalty_y && ineq_constr_resid > settings.dual_tolerance) {
            index_t num_Σ_changed = update_penalty_y(ctx, backend, Σ, e, e_old, settings);
            if (num_Σ_changed > 0)
                timed(stats.timings.update_penalty,
                      [&] { backend.update_penalty_changed(ctx, Σ, num_Σ_changed); });
        }
        // Update regularization
        real_t S_old = std::exchange(S, update_penalty_x(S, settings));
        if (S != S_old) {
            timed(stats.timings.update_regularization, [&] {
                backend.update_regularization_changed(ctx, S, S_old); //
            });
        }
        // Update multipliers
        swap(y, ŷ);
        // Update tolerances
        inner_tol = std::max(inner_tol * settings.ρ, settings.tolerance);
    }
}

template <class Backend>
index_t Solver<Backend>::get_num_variables() const {
    return backend->num_var();
}
template <class Backend>
index_t Solver<Backend>::get_num_equality_constraints() const {
    return backend->num_eq_constr();
}
template <class Backend>
index_t Solver<Backend>::get_num_inequality_constraints() const {
    return backend->num_ineq_constr();
}
template <class Backend>
bool Solver<Backend>::has_result() const {
    return static_cast<bool>(impl);
}
template <class Backend>
void Solver<Backend>::get_solution(std::span<real_t> x) const {
    assert(impl);
    backend->unscale_variables(impl->x, x);
}
template <class Backend>
std::vector<real_t> Solver<Backend>::get_solution() const {
    std::vector<real_t> x(backend->num_var());
    get_solution(x);
    return x;
}
template <class Backend>
void Solver<Backend>::get_equality_multipliers(std::span<real_t> λ) const {
    assert(impl);
    backend->unscale_eq_constr(impl->λ, λ);
}
template <class Backend>
std::vector<real_t> Solver<Backend>::get_equality_multipliers() const {
    std::vector<real_t> λ(backend->num_eq_constr());
    get_equality_multipliers(λ);
    return λ;
}
template <class Backend>
void Solver<Backend>::get_equality_constraints(std::span<real_t> Mxb) const {
    assert(impl);
    backend->unscale_eq_constr(impl->Mxb, Mxb);
}
template <class Backend>
std::vector<real_t> Solver<Backend>::get_equality_constraints() const {
    std::vector<real_t> Mxb(backend->num_eq_constr());
    get_equality_constraints(Mxb);
    return Mxb;
}
template <class Backend>
void Solver<Backend>::get_inequality_multipliers(std::span<real_t> y) const {
    assert(impl);
    backend->unscale_ineq_constr(impl->y, y);
}
template <class Backend>
std::vector<real_t> Solver<Backend>::get_inequality_multipliers() const {
    std::vector<real_t> y(backend->num_ineq_constr());
    get_inequality_multipliers(y);
    return y;
}
template <class Backend>
void Solver<Backend>::get_inequality_constraints(std::span<real_t> Ax) const {
    assert(impl);
    backend->unscale_ineq_constr(impl->Ax, Ax);
}
template <class Backend>
std::vector<real_t> Solver<Backend>::get_inequality_constraints() const {
    std::vector<real_t> x(backend->num_ineq_constr());
    get_inequality_constraints(x);
    return x;
}
template <class Backend>
void Solver<Backend>::warm_start_solution() {
    assert(impl);
    backend->warm_start(impl->x, impl->y, impl->λ);
}
template <class Backend>
void Solver<Backend>::set_initial_guess(std::span<const real_t> x, std::span<const real_t> y,
                                        std::span<const real_t> λ) {
    if (!backend->x0)
        backend->x0.emplace(backend->var_vec());
    backend->scale_variables(x, *(backend->x0));
    if (!backend->y0)
        backend->y0.emplace(backend->ineq_constr_vec());
    backend->scale_ineq_constr(y, *(backend->y0));
    if (!backend->λ0)
        backend->λ0.emplace(backend->eq_constr_vec());
    backend->scale_eq_constr(λ, *(backend->λ0));
}
template <class Backend>
bool Solver<Backend>::get_initial_guess(std::span<real_t> x, std::span<real_t> y,
                                        std::span<real_t> λ) {
    if (!backend->x0 || !backend->y0 || !backend->λ0)
        return false;
    backend->unscale_variables(*(backend->x0), x);
    backend->unscale_ineq_constr(*(backend->y0), y);
    backend->unscale_eq_constr(*(backend->λ0), λ);
    return true;
}
template <class Backend>
void Solver<Backend>::set_b_eq(std::span<const real_t> b_eq) {
    backend->set_b_eq(b_eq);
}
template <class Backend>
void Solver<Backend>::set_b_lb(std::span<const real_t> b_lb) {
    backend->set_b_lb(b_lb);
}
template <class Backend>
void Solver<Backend>::set_b_ub(std::span<const real_t> b_ub) {
    backend->set_b_ub(b_ub);
}
// template <class Backend>
// void Solver<Backend>::get_penalty_factors(std::span<real_t>) const {
//     backend->unscale(...);
// }
// template <class Backend>
// std::vector<real_t> Solver<Backend>::get_penalty_factors() const {
//     std::vector<real_t> x(backend->num_ineq_constr());
//     get_penalty_factors(x);
//     return x;
// }

template <class Backend>
Solver<Backend>::Solver(Backend backend, Settings settings)
    : backend{std::move(backend)}, settings{std::move(settings)} {}
template <class Backend>
Solver<Backend>::Solver(Solver &&) noexcept = default;
template <class Backend>
Solver<Backend> &Solver<Backend>::operator=(Solver &&) noexcept = default;
template <class Backend>
Solver<Backend>::~Solver() = default;

} // namespace CYQLONE_NS(cyqlone::qpalm)
