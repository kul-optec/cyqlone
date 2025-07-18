#pragma once

#include <batmat/matrix/matrix.hpp>
#include <cyqlone/ocp.hpp>
#include <span>
#include <vector>

namespace cyqlone {

using batmat::index_t;
using batmat::real_t;

///                ₙ₋₁
///     minimize    ∑ [½ uᵢᵀ Rᵢ uᵢ + uᵢᵀ S xᵢ + ½ xᵢᵀ Qᵢ xᵢ + rᵢᵀuᵢ + qᵢᵀxᵢ]
///                ⁱ⁼⁰
///                + ½ xₙᵀ Qₙ xₙ + qₙᵀ xₙ
///     s.t.        x₀   = xᵢₙᵢₜ
///                 xᵢ₊₁ = Aᵢ xᵢ + Bᵢ uᵢ + cᵢ
///                 lᵢ   ≤ Cᵢ xᵢ + Dᵢ uᵢ ≤ uᵢ
///                 lₙ   ≤ Cₙ xₙ ≤ uₙ
///
///                ₙ₋₁
///     minimize    ∑ [½ uᵢᵀ Rᵢ uᵢ + uᵢᵀ Sᵢ xᵢ + ½ xᵢᵀ Qᵢ xᵢ + rᵢᵀuᵢ + qᵢᵀxᵢ]
///                ⁱ⁼¹
///                + ½ u₀ᵀ R₀ u₀ + (r₀ + S₀ x₀)ᵀ u₀
///                + ½ xₙᵀ Qₙ xₙ + qₙᵀ xₙ
///     s.t.        x₀   = xᵢₙᵢₜ
///                 xᵢ₊₁ = Aᵢ xᵢ + Bᵢ uᵢ + cᵢ
///                 lᵢ   ≤ Cᵢ xᵢ + Dᵢ uᵢ ≤ uᵢ
///                 l₀ - C₀ x₀ ≤ D₀ U₀ ≤ u₀ - C₀ x₀
///                 lₙ   ≤ Cₙ xₙ ≤ uₙ
template <class T = real_t>
struct CyqloneStorage {
    using value_type = T;
    index_t N_horiz;
    index_t nx, nu, ny, ny_0, ny_N;
    /// Storage layout:         size                 offset
    ///     N × [ R  S ] = H    (nu+nx)²             0
    ///         [ Sᵀ Q ]
    ///     N × [ B  A ]        nx(nu+nx)            N (nu+nx)²
    /// (N-1) × [ D  C ]        ny(nu+nx)            N (nu+nx)² + N nx(nu+nx)
    ///     1 × [ D  C ]        (ny_0+ny_N)(nu+nx)   N (nu+nx)² + N nx(nu+nx) + (N-1)ny(nu+nx)
    using matrix  = batmat::matrix::Matrix<value_type, index_t>;
    matrix data_H = [this] {
        return matrix{{.depth = N_horiz, .rows = nu + nx, .cols = nu + nx}};
    }();
    matrix data_F = [this] { return matrix{{.depth = N_horiz, .rows = nx, .cols = nu + nx}}; }();
    matrix data_G = [this] {
        return matrix{{.depth = N_horiz - 1, .rows = ny, .cols = nu + nx}};
    }();
    matrix data_G0N = [this] {
        return matrix{{.depth = 1, .rows = ny_0 + ny_N, .cols = nu + nx}};
    }();
    matrix data_rq   = [this] { return matrix{{.depth = N_horiz, .rows = nu + nx, .cols = 1}}; }();
    matrix data_c    = [this] { return matrix{{.depth = N_horiz, .rows = nx, .cols = 1}}; }();
    matrix data_lb   = [this] { return matrix{{.depth = N_horiz - 1, .rows = ny, .cols = 1}}; }();
    matrix data_lb0N = [this] { return matrix{{.depth = 1, .rows = ny_0 + ny_N, .cols = 1}}; }();
    matrix data_ub   = [this] { return matrix{{.depth = N_horiz - 1, .rows = ny, .cols = 1}}; }();
    matrix data_ub0N = [this] { return matrix{{.depth = 1, .rows = ny_0 + ny_N, .cols = 1}}; }();
    std::vector<index_t> indices_G0 = std::vector<index_t>(ny_0);

    static CyqloneStorage build(const LinearOCPStorage &ocp, std::span<const value_type> qr,
                                  std::span<const value_type> b_eq,
                                  std::span<const value_type> b_lb,
                                  std::span<const value_type> b_ub);
    static void reconstruct_ineq_multipliers(const LinearOCPStorage &ocp,
                                             std::span<const value_type> y_compressed,
                                             std::span<value_type> y);
};

} // namespace cyqlone
