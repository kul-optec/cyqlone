#include <cyqlone/conversion.hpp>
#include <cyqlone/qpalm/example-problems/platooning.hpp>
#include <cyqlone/qpalm/example-problems/zoh.hpp>
#include <guanaqo/eigen/view.hpp>

#include <algorithm>
#include <limits>

namespace CYQLONE_NS(cyqlone::qpalm::problems) {

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
    real_t scal   = p.scale_cost / static_cast<real_t>(p.N_horiz);

    // Constraints rhs
    ocp.b().set_constant(0);
    for (index_t v = 0; v < n_vehicle; ++v)
        ocp.b(0)(2 * v, 0) = -static_cast<real_t>(v) * p.dist_init;
    ocp.b_min().set_constant(-inf);
    ocp.b_max().set_constant(+inf);

    // Dynamics and constraint matrices
    for (index_t i = 0; i < N; ++i) {
        auto Ai = ocp.A(i), Bi = ocp.B(i), Ci = ocp.C(i), Di = ocp.D(i);
        auto Qi = ocp.Q(i), Ri = ocp.R(i);
        auto lbi = ocp.b_min(i), ubi = ocp.b_max(i);
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
            Qi(2 * v, 2 * v)         = scal * 10;
            Qi(2 * v + 1, 2 * v + 1) = scal * 1;
            Ri(v, v)                 = scal * 5;
        }
    }
    auto Ci = ocp.C(N), Qi = ocp.Q(N);
    auto lbi = ocp.b_min(N), ubi = ocp.b_max(N);
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
        Qi(2 * v, 2 * v)         = scal * 100;
        Qi(2 * v + 1, 2 * v + 1) = scal * 1;
    }
    std::vector<real_t> ref(nx + nu + nx);
    for (index_t v = 0; v < n_vehicle; ++v) {
        ref[2 * v]           = p.p_target;
        ref[2 * v + nx + nu] = p.p_target;
    }
    reference_to_gradient(ocp, ref);

    return {
        .ocp = std::move(ocp),
        .ref = std::move(ref),
    };
}

} // namespace CYQLONE_NS(cyqlone::qpalm::problems)
