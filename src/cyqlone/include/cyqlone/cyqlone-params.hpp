#pragma once

#include <cyqlone/config.hpp>
#include <limits>
#include <stdexcept>

namespace cyqlone {

/// Defines how to solve the small linear system that remains after cyclic reduction.
enum class SolveMethod {
    StairPCG,  ///< Preconditioned Conjugate Gradient with staircase preconditioner (iterative)
    JacobiPCG, ///< Preconditioned Conjugate Gradient with Jacobi preconditioner (iterative)
    PCR,       ///< Parallel Cyclic Reduction (direct)
};

inline const char *enum_name(SolveMethod s) {
    switch (s) {
        case SolveMethod::StairPCG: return "StairPCG";
        case SolveMethod::JacobiPCG: return "JacobiPCG";
        case SolveMethod::PCR: return "PCR";
        default:;
    }
    throw std::out_of_range("invalid value for cyqlone::SolveMethod");
}

/// Parameters and settings for the Cyqlone solver.
template <class T = real_t>
struct CyqloneParams {
    using value_type = T;

    /// Use prefetching during the reverse CR solve phase.
    bool enable_prefetching = true;
    /// Maximum number of preconditioned conjugate gradient iterations.
    index_t pcg_max_iter = 100;
    /// Tolerance for the preconditioned conjugate gradient solver.
    value_type pcg_tolerance = std::numeric_limits<value_type>::epsilon() / 10;
    /// Enable printing of the residuals during PCG.
    bool pcg_print_resid = false;
    /// Algorithm to use for solving the final reduced block tridiagonal system.
    SolveMethod solve_method = SolveMethod::StairPCG;
    /// Tuning parameter for deciding when to update or re-factor the PCR factorization.
    /// If the update rank exceeds this fraction of nx, the PCR factorization is recomputed
    double pcr_max_update_fraction = 0.6;
    /// Tuning parameter for deciding when to update or re-factor the last subdiagonal blocks in the
    /// CR factorization.
    /// If the update rank exceeds this fraction of nx, the last subdiagonal blocks are
    /// recomputed.
    /// @todo Add option to switch at any level of CR, not just the last one.
    double cr_max_update_fraction_Y0 = 0.9;
    /// Threshold on nx for switching to a serial implementation of the reverse CR solve.
    index_t parallel_solve_cr_threshold = 10;
};

} // namespace cyqlone
