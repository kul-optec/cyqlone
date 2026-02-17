#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/qpalm/export.h>
#include <iosfwd>
#include <stdexcept>

namespace cyqlone::qpalm {

/// Exit status of a numerical solver.
/// @ingroup topic-optimization-solvers
enum class SolverStatus {
    Busy = 0,    ///< In progress.
    Converged,   ///< Converged and reached given tolerance.
    MaxTime,     ///< Maximum allowed execution time exceeded.
    MaxIter,     ///< Maximum number of iterations exceeded.
    NotFinite,   ///< Intermediate results were infinite or not-a-number.
    NoProgress,  ///< No progress was made in the last iteration.
    Interrupted, ///< Solver was interrupted by the user.
    Exception,   ///< An unexpected exception was thrown.
};

inline const char *enum_name(SolverStatus s) {
    switch (s) {
        case SolverStatus::Busy: return "Busy";
        case SolverStatus::Converged: return "Converged";
        case SolverStatus::MaxTime: return "MaxTime";
        case SolverStatus::MaxIter: return "MaxIter";
        case SolverStatus::NotFinite: return "NotFinite";
        case SolverStatus::NoProgress: return "NoProgress";
        case SolverStatus::Interrupted: return "Interrupted";
        case SolverStatus::Exception: return "Exception";
        default:;
    }
    throw std::out_of_range("invalid value for qpalm::SolverStatus");
}

CYQLONE_QPALM_EXPORT std::ostream &operator<<(std::ostream &os, SolverStatus s);

} // namespace cyqlone::qpalm
