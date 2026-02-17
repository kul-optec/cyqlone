#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/qpalm/export.h>
#include <chrono>

namespace cyqlone::qpalm {

using namespace std::chrono_literals;

/// @ingroup topic-optimization-solvers
struct CYQLONE_QPALM_EXPORT Settings {
    /// Maximum number of (total) iterations.
    unsigned max_outer_iter            = 100;
    unsigned max_inner_iter            = 100;
    unsigned max_total_inner_iter      = 10000;
    std::chrono::microseconds max_time = 5min;
    /// Primal tolerance.
    real_t tolerance                       = real_t(1e-8);
    real_t dual_tolerance                  = real_t(1e-8);
    real_t eq_constr_tolerance             = real_t(1e-10);
    real_t initial_inner_tolerance         = real_t(1);
    real_t ρ                               = real_t(1e-1);
    real_t θ                               = 0.25;
    real_t Δy                              = 100;
    real_t Δy_always                       = 1;
    real_t max_penalty_y                   = 1e9;
    real_t initial_penalty_y               = 20;
    bool scale_initial_penalty_y           = false;
    real_t Δx                              = 10;
    real_t max_penalty_x                   = 1e7;
    real_t boost_penalty_x                 = 1e12;
    real_t initial_penalty_x               = 1e7;
    bool proximal                          = true;
    bool recompute_eq_res                  = true; // TODO
    bool recompute_inner                   = false;
    bool recompute                         = true;
    bool verbose                           = true;
    int print_precision                    = 3;
    unsigned max_no_changes_active_set     = 5;
    bool linesearch_include_multipliers    = true; // TODO
    bool force_linesearch_if_no_set_change = true;
    bool force_linesearch_if_dir_deriv_pos = false;
    bool detailed_stats                    = false;
    bool scale_newton_step                 = false; // TODO: can probably be removed
    bool print_directional_deriv           = false;
    bool print_linesearch_inputs           = false;

    // TODO
    bool operator==(const Settings &) const { return false; }
    bool operator!=(const Settings &other) const { return !(*this == other); }
};

} // namespace cyqlone::qpalm
