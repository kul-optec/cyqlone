/**
 * @file
 * Conversion utilities for optimal control problems.
 * For example: converting an OCP into a sparse “multiple shooting”
 * quadratic program, or computing the gradient of the quadratic OCP
 * cost function.
 */

#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/ocp.hpp>
#include <guanaqo/linalg/sparsity.hpp>
#include <vector>

namespace cyqlone::qpalm {

using guanaqo::linalg::sparsity::SparseCSC;

/// Represents a sparse multiple shooting formulation of the standard optimal
/// control problem.
struct LinearOCPSparseQP {
    std::vector<index_t> Q_outer_ptr, Q_inner_idx;
    std::vector<real_t> Q_values;
    SparseCSC<index_t, index_t> Q_sparsity;
    std::vector<index_t> A_outer_ptr, A_inner_idx;
    std::vector<real_t> A_values;
    SparseCSC<index_t, index_t> A_sparsity;
    index_t n, m_eq, m_ineq;

    [[nodiscard]] static LinearOCPSparseQP build(const LinearOCPStorage &ocp);

    struct KKTMatrix {
        std::vector<index_t> outer_ptr, inner_idx;
        std::vector<real_t> values;
        SparseCSC<index_t, index_t> sparsity;
    };
    /// Returns the lower part of the symmetric indefinite KKT matrix for the
    /// active set @p J and penalty factors @p Σ.
    [[nodiscard]] KKTMatrix build_kkt(real_t S, std::span<const real_t> Σ,
                                      std::span<const bool> J) const;
};

} // namespace cyqlone::qpalm

namespace CYQLONE_NS(cyqlone::qpalm) {

/// Simply computes the gradient of the quadratic cost
/// @f$ J(x, u) = \sum_{j=1}^{N-1} \ell_j(x^j, u^j) + \ell_N(x^N) @f$,
/// with @f$ \ell_j(x, u) = \tfrac12 \left\| \begin{pmatrix} x - x^j_\text{ref}
/// \\ u - u^j_\text{ref} \right\|_{H_j}^2 @f$, with the Hessian
/// @f$ H_j = \begin{pmatrix} Q_j & S_j^\top \\ S_j & R_j \end{pmatrix} @f$.
/// Stores @f$ \nabla J(0, 0) @f$ to @p qr.
void reference_to_gradient(const LinearOCPStorage &ocp, std::span<const real_t> ref,
                           std::span<real_t> qr);
void reference_to_gradient(LinearOCPStorage &ocp, std::span<const real_t> ref);

} // namespace CYQLONE_NS(cyqlone::qpalm)
