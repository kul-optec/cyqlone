#pragma once

#include <cyqlone/ocp.hpp>
#include <batmat/matrix/matrix.hpp>
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
    std::vector<bool> Ju0;
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

    void update_impl(const LinearOCPStorage &ocp);
    void update(const LinearOCPStorage &ocp);
    static CyqloneStorage build(const LinearOCPStorage &ocp, index_t ny_0 = -1);
    static void reconstruct_ineq_multipliers(const LinearOCPStorage &ocp,
                                             std::span<const value_type> y_compressed,
                                             std::span<value_type> y);
    static index_t count_constr_0(const LinearOCPStorage &ocp, std::vector<bool> &Ju0);
};

} // namespace cyqlone
