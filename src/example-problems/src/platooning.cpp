#include <cyqlone/qpalm/example-problems/platooning.hpp>
#include <cyqlone/qpalm/example-problems/zoh.hpp>
#include <guanaqo/eigen/view.hpp>

#include <algorithm>
#include <limits>

namespace cyqlone::qpalm::problems {

PlatooningProblem platooning(PlatooningParams p) {
    using guanaqo::as_view;
    const auto inf = std::numeric_limits<real_t>::infinity();

    auto n_vehicle = static_cast<index_t>(p.masses.size());
    LinearOCPStorage ocp{.dim{
        .N_horiz = p.N_horiz,
        .nx      = 2 * n_vehicle,
        .nu      = n_vehicle,
        .ny      = 2 * n_vehicle + (n_vehicle - 1),
        .ny_N    = n_vehicle + (n_vehicle - 1),
    }};
    auto [N, nx, nu, ny, ny_N] = ocp.dim;

    // Continuous-time dynamics
    eigen_mat A = eigen_mat::Zero(nx, nx), B = eigen_mat::Zero(nx, nu);
    for (Eigen::Index v = 0; v < n_vehicle; ++v) {
        A(2 * v, 2 * v + 1)     = 1;               // Velocity → position
        A(2 * v + 1, 2 * v + 1) = -p.friction;     // Friction
        B(2 * v + 1, v)         = 1 / p.masses[v]; // Accel → velocity
    }
    auto Ts       = p.T_horiz / static_cast<real_t>(p.N_horiz);
    auto [Ad, Bd] = discretize_zoh(A, B, Ts);

    // Constraints rhs
    std::vector<real_t> eq((N + 1) * nx), lb(N * ny + ny_N), ub(N * ny + ny_N);
    for (index_t v = 0; v < n_vehicle; ++v)
        eq[2 * v] = -static_cast<real_t>(v) * p.dist_init;
    std::ranges::fill(lb, -inf);
    std::ranges::fill(ub, +inf);

    // Dynamics and constraint matrices
    for (index_t i = 0; i < N; ++i) {
        auto Ai = ocp.A(i), Bi = ocp.B(i), Ci = ocp.C(i), Di = ocp.D(i);
        auto Qi = ocp.Q(i), Ri = ocp.R(i);
        auto lbi = guanaqo::MatrixView<real_t, index_t>::as_column(lb).middle_rows(i * ny, ny);
        auto ubi = guanaqo::MatrixView<real_t, index_t>::as_column(ub).middle_rows(i * ny, ny);
        for (index_t v = 0; v < n_vehicle; ++v) {
            Ai                            = as_view(Ad);
            Bi                            = as_view(Bd);
            Ci(v, 2 * v + 1)              = 1; // Measure velocity
            lbi(v, 0)                     = -p.v_max;
            ubi(v, 0)                     = +p.v_max;
            Di(2 * n_vehicle - 1 + v, v)  = 1; // Measure input
            lbi(2 * n_vehicle - 1 + v, 0) = -p.F_max;
            ubi(2 * n_vehicle - 1 + v, 0) = +p.F_max;
            if (v > 0) {
                // p[0] + Ts v[0] ≥ p[1] + v[1] + d
                // p[v] - p[v-1] + Ts (v[k] - v[k-1]) ≤ -d
                Ci(n_vehicle + v - 1, 2 * v)           = 1;
                Ci(n_vehicle + v - 1, 2 * (v - 1))     = -1;
                Ci(n_vehicle + v - 1, 2 * v + 1)       = Ts;
                Ci(n_vehicle + v - 1, 2 * (v - 1) + 1) = -Ts;
                lbi(n_vehicle + v - 1, 0)              = -inf;
                ubi(n_vehicle + v - 1, 0)              = -p.dist_min;
            }
            Qi(2 * v, 2 * v)         = 10;
            Qi(2 * v + 1, 2 * v + 1) = 1;
            Ri(v, v)                 = 5;
        }
    }
    auto Ci = ocp.C(N), Qi = ocp.Q(N);
    auto lbi = guanaqo::MatrixView<real_t, index_t>::as_column(lb).middle_rows(N * ny, ny_N);
    auto ubi = guanaqo::MatrixView<real_t, index_t>::as_column(ub).middle_rows(N * ny, ny_N);
    for (index_t v = 0; v < n_vehicle; ++v) {
        Ci(v, 2 * v + 1) = 1; // Measure velocity
        lbi(v, 0)        = -p.v_max;
        ubi(v, 0)        = +p.v_max;
        if (v > 0) {
            // p[v] - p[v-1] + Ts (v[k] - v[k-1]) ≤ -d
            Ci(n_vehicle + v - 1, 2 * v)           = 1;
            Ci(n_vehicle + v - 1, 2 * (v - 1))     = -1;
            Ci(n_vehicle + v - 1, 2 * v + 1)       = Ts;
            Ci(n_vehicle + v - 1, 2 * (v - 1) + 1) = -Ts;
            lbi(n_vehicle + v - 1, 0)              = -inf;
            ubi(n_vehicle + v - 1, 0)              = -p.dist_min;
        }
        Qi(2 * v, 2 * v)         = 100;
        Qi(2 * v + 1, 2 * v + 1) = 1;
    }
    std::vector<real_t> ref(nx + nu + nx);
    for (index_t v = 0; v < n_vehicle; ++v) {
        ref[2 * v]           = p.p_target;
        ref[2 * v + nx + nu] = p.p_target;
    }

    return {
        .ocp         = std::move(ocp),
        .rhs_eq      = std::move(eq),
        .rhs_ineq_lb = std::move(lb),
        .rhs_ineq_ub = std::move(ub),
        .ref         = std::move(ref),
    };
}

} // namespace cyqlone::qpalm::problems
