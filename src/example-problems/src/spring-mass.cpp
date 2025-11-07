#include <cyqlone/qpalm/example-problems/conversion.hpp>
#include <cyqlone/qpalm/example-problems/spring-mass.hpp>
#include <cyqlone/qpalm/example-problems/zoh.hpp>
#include <batmat/config.hpp>
#include <guanaqo/eigen/view.hpp>

#include <algorithm>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>

namespace CYQLONE_NAMESPACE::qpalm::problems {

SpringMassProblem spring_mass(SpringMassParams p) {
    using guanaqo::as_view;
    const auto inf = std::numeric_limits<real_t>::infinity();

    auto n_masses = static_cast<index_t>(p.masses.size());
    LinearOCPStorage ocp{.dim{
        .N_horiz = p.N_horiz,
        .nx      = 2 * n_masses, // position + velocity for each mass
        .nu      = p.n_actuators,
        .ny      = n_masses + p.n_actuators,
        .ny_N    = n_masses,
    }};
    auto [N, nx, nu, ny, ny_N] = ocp.dim;

    // Continuous-time dynamics
    eigen_mat A = eigen_mat::Zero(nx, nx), B = eigen_mat::Zero(nx, nu), b = eigen_mat::Zero(nx, 1);
    for (Eigen::Index v = 0; v < n_masses; ++v) {
        const auto km                 = p.k_spring / p.masses[v];
        A(v, n_masses + v)            = 1;           // ṗ(t) = v(t)
        A(n_masses + v, n_masses + v) = -p.friction; // v̇(t) = -ρ v(t) + ...
        A(n_masses + v, v)            = -2 * km;     // ... - (2k/m) p(t) + ...
        if (v > 0)
            A(n_masses + v, v - 1) = km; // ... + (k/m) p_{v-1}(t)
        if (v < n_masses - 1)
            A(n_masses + v, v + 1) = km; // ... + (k/m) p_{v+1}(t)
        else
            b(n_masses + v, 0) = km * p.width; // Right wall force
    }
    switch (p.actuator_placement) {
        case SpringMassParams::IndividualActuators:
            for (Eigen::Index a = 0; a < p.n_actuators; ++a)
                B(n_masses + a, a) = 1 / p.masses[a];
            break;
        case SpringMassParams::RandomActuators: {
            std::mt19937 rng(p.seed);
            std::vector<index_t> actuator_indices(n_masses);
            std::ranges::iota(actuator_indices, index_t{});
            std::shuffle(actuator_indices.begin(), actuator_indices.end(), rng);
            for (Eigen::Index a = 0; a < p.n_actuators; ++a)
                B(n_masses + actuator_indices[a], a) = 1 / p.masses[actuator_indices[a]];
        } break;
        case SpringMassParams::RandomPairsOfActuators: {
            std::mt19937 rng(p.seed);
            std::vector<index_t> actuator_indices_l(n_masses), actuator_indices_r(n_masses);
            std::ranges::iota(actuator_indices_l, index_t{});
            std::ranges::iota(actuator_indices_r, index_t{});
            std::shuffle(actuator_indices_l.begin(), actuator_indices_l.end(), rng);
            std::shuffle(actuator_indices_r.begin(), actuator_indices_r.end(), rng);
            for (Eigen::Index a = 0; a < p.n_actuators; ++a) {
                B(n_masses + actuator_indices_l[a], a) = +1 / p.masses[actuator_indices_l[a]];
                B(n_masses + actuator_indices_r[a], a) = -1 / p.masses[actuator_indices_r[a]];
            }
        } break;
        case SpringMassParams::WangBoydActuators:
            for (Eigen::Index a = 0; a < p.n_actuators; ++a) {
                if (a % 3 == 0) {
                    auto v_l = 2 * a, v_r = 2 * a + 1;
                    if (v_l < n_masses)
                        B(n_masses + v_l, a) = +1 / p.masses[v_l];
                    if (v_r < n_masses)
                        B(n_masses + v_r, a) = -1 / p.masses[v_r];
                } else if (a % 3 == 1) {
                    auto v_l = 2 * a, v_r = 2 * a + 2;
                    if (v_l < n_masses)
                        B(n_masses + v_l, a) = +1 / p.masses[v_l];
                    if (v_r < n_masses)
                        B(n_masses + v_r, a) = -1 / p.masses[v_r];
                } else {
                    auto v_l = 2 * a - 1, v_r = 2 * a + 1;
                    if (v_l < n_masses)
                        B(n_masses + v_l, a) = +1 / p.masses[v_l];
                    if (v_r < n_masses)
                        B(n_masses + v_r, a) = -1 / p.masses[v_r];
                }
            }
            break;
        default: throw std::invalid_argument("Invalid actuator placement");
    }
    auto Ts = p.T_horiz / static_cast<real_t>(p.N_horiz);
    // ẋ = Ax + Bu + b  →  x[k+1] = Ad x[k] + Bd u[k] + bd
    auto [Ad, Bd, bd] = discretize_zoh(A, B, b, Ts);

    // Constraints rhs
    ocp.b().set_constant(0);
    ocp.b_min().set_constant(-inf);
    ocp.b_max().set_constant(+inf);
    auto x0 = ocp.b(0);
    for (index_t v = 0; v < n_masses; ++v) // Initial positions
        x0(v, 0) = static_cast<real_t>(v + 1) * p.width / static_cast<real_t>(n_masses + 1);

    // Dynamics and constraint matrices
    for (index_t i = 0; i < N; ++i) {
        auto Ai = ocp.A(i), Bi = ocp.B(i), bi = ocp.b(i + 1), Ci = ocp.C(i), Di = ocp.D(i);
        auto Qi = ocp.Q(i), Ri = ocp.R(i);
        auto lbi = ocp.b_min(i), ubi = ocp.b_max(i);
        for (index_t v = 0; v < n_masses; ++v) {
            Ai                             = as_view(Ad);
            Bi                             = as_view(Bd);
            bi                             = as_view(bd);
            Ci(v, v)                       = 1; // Measure displacement
            lbi(v, 0)                      = x0(v, 0) - p.p_max;
            ubi(v, 0)                      = x0(v, 0) + p.p_max;
            Qi(v, v)                       = p.q_pos;
            Qi(n_masses + v, n_masses + v) = p.q_vel;
        }
        for (index_t a = 0; a < p.n_actuators; ++a) {
            Di(n_masses + a, a)  = 1; // Measure input
            lbi(n_masses + a, 0) = -p.F_max;
            ubi(n_masses + a, 0) = +p.F_max;
            Ri(a, a)             = p.r_act;
        }
    }
    auto Ci = ocp.C(N), Qi = ocp.Q(N);
    auto lbi = ocp.b_min(N), ubi = ocp.b_max(N);
    for (index_t v = 0; v < n_masses; ++v) {
        Ci(v, v)                       = 1; // Measure displacement
        lbi(v, 0)                      = x0(v, 0) - p.p_max;
        ubi(v, 0)                      = x0(v, 0) + p.p_max;
        Qi(v, v)                       = p.q_pos;
        Qi(n_masses + v, n_masses + v) = p.q_vel;
    }
    std::vector<real_t> ref(nx + nu + nx);
    for (index_t v = 0; v < n_masses; ++v) {
        ref[v]           = x0(v, 0); // position
        ref[nx + nu + v] = x0(v, 0); // final position
    }
    reference_to_gradient(ocp, ref);

    return {
        .ocp = std::move(ocp),
        .ref = std::move(ref),
    };
}

} // namespace CYQLONE_NAMESPACE::qpalm::problems
