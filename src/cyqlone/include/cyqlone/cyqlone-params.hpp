#pragma once

#include <cyqlone/config.hpp>
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

} // namespace cyqlone
