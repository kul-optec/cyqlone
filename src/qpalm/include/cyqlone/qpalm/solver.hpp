#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/qpalm/detailed-stats.hpp>
#include <cyqlone/qpalm/export.h>
#include <cyqlone/qpalm/settings.hpp>
#include <cyqlone/qpalm/status.hpp>
#include <cyqlone/timing.hpp>
#include <guanaqo/atomic-stop-signal.hpp>
#include <guanaqo/timed.hpp>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace CYQLONE_NAMESPACE::qpalm {

struct CYQLONE_QPALM_EXPORT SolverTimings {
    DefaultTimings total;
    DefaultTimings scaling;
    DefaultTimings line_search;
    DefaultTimings recompute_inner;
    DefaultTimings recompute_outer;
    DefaultTimings mat_vec_M;
    DefaultTimings mat_vec_MT;
    DefaultTimings mat_vec_A;
    DefaultTimings mat_vec_AT;
    DefaultTimings mat_vec_Q;
    DefaultTimings active_set_change;
    DefaultTimings update_penalty;
    DefaultTimings update_regularization;
    DefaultTimings boost_regularization;
    DefaultTimings solve;
    std::map<std::string, DefaultTimings> backend;
};

CYQLONE_QPALM_EXPORT std::ostream &operator<<(std::ostream &, const SolverTimings &);

struct CYQLONE_QPALM_EXPORT SolverStats {
    unsigned inner_iter         = 0;
    unsigned outer_iter         = 0;
    real_t stationarity         = std::numeric_limits<real_t>::quiet_NaN();
    real_t primal_residual_norm = std::numeric_limits<real_t>::quiet_NaN();
    real_t max_penalty          = std::numeric_limits<real_t>::quiet_NaN();
    SolverTimings timings{};
    std::optional<DetailedStats> detail = std::nullopt;
};

namespace detail {

template <class T>
struct backend_type; // deliberately undefined

template <class T, class D>
struct backend_type<std::unique_ptr<T, D>> {
    using type = T;
};

template <class T>
struct backend_type<T *> {
    using type = T;
};

template <class T>
using backend_type_t = backend_type<T>::type;

} // namespace detail

template <class Backend>
struct SolverImplementation;

template <class Backend>
class Solver {
  private:
    using backend_type = detail::backend_type_t<Backend>;
    /// Run QPALM on the scaled problem
    [[nodiscard]] SolverStatus do_solve();

  public:
    SolverStatus operator()() {
        guanaqo::Timed t{stats.emplace().timings.total};
        return do_solve();
    }

    Backend backend;
    Settings settings;
    std::unique_ptr<SolverImplementation<backend_type>> impl{};
    std::optional<SolverStats> stats = std::nullopt;
    guanaqo::AtomicStopSignal stop_signal{};

    [[nodiscard]] index_t get_num_variables() const;
    [[nodiscard]] index_t get_num_equality_constraints() const;
    [[nodiscard]] index_t get_num_inequality_constraints() const;

    [[nodiscard]] bool has_result() const;
    void get_solution(std::span<real_t>) const;
    [[nodiscard]] std::vector<real_t> get_solution() const;
    void get_equality_multipliers(std::span<real_t>) const;
    [[nodiscard]] std::vector<real_t> get_equality_multipliers() const;
    void get_equality_constraints(std::span<real_t>) const;
    [[nodiscard]] std::vector<real_t> get_equality_constraints() const;
    void get_inequality_multipliers(std::span<real_t>) const;
    [[nodiscard]] std::vector<real_t> get_inequality_multipliers() const;
    void get_inequality_constraints(std::span<real_t>) const;
    [[nodiscard]] std::vector<real_t> get_inequality_constraints() const;
    void get_penalty_factors(std::span<real_t>) const;
    [[nodiscard]] std::vector<real_t> get_penalty_factors() const;
    void warm_start_solution();
    void set_initial_guess(std::span<const real_t> x, std::span<const real_t> y,
                           std::span<const real_t> λ);
    bool get_initial_guess(std::span<real_t> x, std::span<real_t> y, std::span<real_t> λ);
    void set_b_eq(std::span<const real_t> b_eq);
    void set_b_lb(std::span<const real_t> b_lb);
    void set_b_ub(std::span<const real_t> b_ub);

    void stop() { stop_signal.stop(); }

    Solver(Backend backend, Settings settings = {});
    Solver(const Solver &)            = delete;
    Solver &operator=(const Solver &) = delete;
    Solver(Solver &&) noexcept;
    Solver &operator=(Solver &&) noexcept;
    ~Solver();
};

} // namespace CYQLONE_NAMESPACE::qpalm
