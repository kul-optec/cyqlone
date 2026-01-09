from dataclasses import dataclass
from typing import Callable, Union
import cyqlone
import numpy as np
import numpy.linalg as la
import pytest

"""
     │ x₄   λ₃   u₃   x₃   λ₂   u₂   x₂   λ₁   u₁   x₁   λ₀   u₀   x₀   λ₋₁
─────┼─────────────────────────────────────────────────────────────────────
 -q₄ │ Q₄  -I
 -f₃ │-I    0    B₃   A₃
 -r₃ │      B₃ᵀ  R₃   S₃
 -q₃ │      A₃ᵀ  S₃ᵀ  Q₃  -I 
 -f₂ │               -I    0    B₂   A₂
 -r₂ │                     B₂ᵀ  R₂   S₂
 -q₂ │                     A₂ᵀ  S₂ᵀ  Q₂  -I
 -f₁ │                              -I    0    B₁   A₁
 -r₁ │                                    B₁ᵀ  R₁   S₁
 -q₁ │                                    A₁ᵀ  S₁ᵀ  Q₁  -I
 -f₀ │                                             -I    0    B₀   A₀
 -r₀ │                                                   B₀ᵀ  R₀   S₀
 -q₀ │                                                   A₀ᵀ  S₀ᵀ  Q₀  -I
 -f₋₁│                                                            -I    0

     │ u₀   x₁   u₁   x₂   u₂   x₃   u₃   x₄   λ₀   λ₁   λ₂   λ₃
─────┼───────────────────────────────────────────────────────────
 -r̃₀ │ R₀                                      B₀ᵀ
 -q₁ |      Q₁   S₁ᵀ                          -I    A₁ᵀ
 -r₁ │      S₁   R₁                                 B₁ᵀ
 -q₂ │                Q₂   S₂ᵀ                     -I    A₂ᵀ
 -r₂ │                S₂   R₂                            B₂ᵀ
 -q₃ │                          Q₃   S₃ᵀ                -I    A₃ᵀ
 -r₃ │                          S₃   R₃                       B₃ᵀ
 -q₄ │                                    Q₄                 -I
 -f̃₀ │ B₀  -I
 -f₁ |      A₁   B₁  -I
 -f₂ │                A₂   B₂  -I
 -f₃ │                          A₃   B₃  -I

     │ z   μ
─────┼───────
 -g  │ Q   Mᵀ
 -b  | M   0

Qz⁺ + Mᵀμ⁺ = -g
Mz⁺        = -b

Q(z+Δz) + Mᵀ(μ+Δμ) = -g
M(z+Δz)            = -b

QΔz + MᵀΔμ = -(Qz + g + Mᵀμ)
MΔz        = -(b + Mz)

"""

Solver = Union[
    cyqlone.scalar.v2.CyqloneSolver, cyqlone.simd4.v2.CyqloneSolver, cyqlone.simd8.v2.CyqloneSolver
]


np.set_printoptions(
    precision=17, suppress=False, linewidth=800, formatter={"float_kind": "{:+.17e}".format}
)

plot_on_failure = False


def random_ocp(N, nx, nu, ny, ny_N, rng):
    QRS = rng.uniform(-1, 1, (N, nx + nu, nx + nu))
    QRS = QRS @ np.transpose(QRS, [0, 2, 1])
    QRS += 1e-3 * np.broadcast_to(np.eye(nx + nu), (N, nx + nu, nx + nu))
    QN = rng.uniform(-1, 1, (nx, nx))
    QN = QN @ QN.T + 1e-3 * np.eye(nx)
    return cyqlone.OCP(
        AB=np.asfortranarray(rng.uniform(-1, 1, (N, nx, nx + nu))),
        CD=np.asfortranarray(rng.uniform(-1, 1, (N, ny, nx + nu))),
        CN=np.asfortranarray(rng.uniform(-1, 1, (ny_N, nx))),
        QRS=np.asfortranarray(QRS),
        QN=np.asfortranarray(QN),
        qr=rng.uniform(-1, 1, N * (nx + nu) + nx),
        rhs_eq=rng.uniform(-1, 1, (N + 1) * nx),  # TODO
        rhs_lb=rng.uniform(-1, 1, N * ny + ny_N),
        rhs_ub=rng.uniform(-1, 1, N * ny + ny_N),
    )


@dataclass
class Params:
    P: int
    N: int
    nx: int = 13
    nu: int = 11
    ny: int = 17
    ny_N: int = 7
    seed: int = 12345
    solver: Callable[[cyqlone.CyqloneOCP, int], Solver] = cyqlone.scalar.v2.CyqloneSolver


def prepare_test(params: Params):
    rng = np.random.default_rng(seed=params.seed)
    ocp = random_ocp(params.N, params.nx, params.nu, params.ny, params.ny_N, rng)
    cocp = cyqlone.CyqloneOCP(ocp)
    solver = params.solver(cocp, params.P)
    solver.solve_method = cyqlone.SolveMethod.PCR
    return rng, ocp, cocp, solver


# TODO: this should really be provided by the solver itself
def extract_solution(solver: Solver, ocp: cyqlone.OCP, ux, λ):
    x0 = ocp.b(0)
    uxs = solver.unpack_variables(ux)
    N, nx, nu = ocp.N_horiz, ocp.nx, ocp.nu
    nux = nu + nx
    u = np.array([uxs[j * nux : nu + j * nux] for j in range(N)])
    x = np.array([uxs[nu + j * nux : nux + j * nux] for j in range(N)])
    x = np.vstack([x0[None, :], x])
    λs = solver.unpack_dynamics(λ).reshape((N, nx))
    λ_init = ocp.q(0) + ocp.Q(0) @ x0 + ocp.S(0).T @ u[0] + ocp.A(0).T @ λs[0]
    λs = np.vstack([λs, λ_init[None, :]])  # λ_init corresponds to λs[-1]
    return u, x, λs


def compute_kkt_residuals(ocp: cyqlone.OCP, u, x, λs):
    N, nx, nu = ocp.N_horiz, ocp.nx, ocp.nu
    r = np.zeros((N, nx))
    gx = np.zeros((N + 1, nx))
    gu = np.zeros((N, nu))
    for j in range(N):
        r[j] = ocp.A(j) @ x[j] + ocp.B(j) @ u[j] + ocp.b(j + 1) - x[j + 1]
        gx[j] = ocp.Q(j) @ x[j] + ocp.S(j).T @ u[j] + ocp.q(j) + ocp.A(j).T @ λs[j] - λs[j - 1]
        gu[j] = ocp.R(j) @ u[j] + ocp.S(j) @ x[j] + ocp.r(j) + ocp.B(j).T @ λs[j]
    gx[N] = ocp.Q(N) @ x[N] + ocp.q(N) - λs[N - 1]
    return r, gx, gu


def check_kkt_residuals(N: int, r, gx, gu):
    for j in range(N):  # Check dynamics residuals
        print(j, " " * 6, end="\r")
        assert la.norm(r[j], np.inf) < 1e-8
    for j in range(N + 1):  # Check state gradient residuals
        print(j, " " * 6, end="\r")
        assert la.norm(gx[j], np.inf) < 1e-10
    for j in range(N):  # Check control gradient residuals
        print(j, " " * 6, end="\r")
        assert la.norm(gu[j], np.inf) < 1e-10


def plot_kkt_residuals(r, gx, gu):
    if not plot_on_failure:
        return  # skip if running as part of pytest
    import matplotlib.pyplot as plt

    plt.figure()
    plt.semilogy(np.abs(r).max(axis=1), ".-", label="Dynamics residuals")
    plt.semilogy(np.abs(gx).max(axis=1), ".-", label="State gradient residuals")
    plt.semilogy(np.abs(gu).max(axis=1), ".-", label="Control gradient residuals")
    plt.xlabel("Stage")
    plt.ylabel("Infinity norm of residual")
    plt.title(f"KKT Residuals for")
    plt.legend()
    plt.grid(True)
    plt.show()


def run_test_cyqlone_factor_solve(params):
    _, ocp, cocp, solver = prepare_test(params)
    print(solver.__class__.__qualname__, solver.params_string)
    Σ = np.zeros(solver.num_general_constraints)
    Σ_packed = solver.pack_constraints(Σ)

    for _ in range(50):
        ux, λ = solver.factor_solve(np.inf, Σ_packed, cocp)
    cyqlone.reset_trace_log()
    solver.log_thread_names()
    ux, λ = solver.factor_solve(np.inf, Σ_packed, cocp)
    solver.solve_reverse(ux, λ)

    u, x, λs = extract_solution(solver, ocp, ux, λ)
    r, gx, gu = compute_kkt_residuals(ocp, u, x, λs)
    try:
        check_kkt_residuals(ocp.N_horiz, r, gx, gu)
    except AssertionError:
        plot_kkt_residuals(r, gx, gu)
        raise
    finally:
        print()
        cyqlone.dump_trace_log(f"traces/test_cyqlone_v2_trace-{solver.params_string}.csv")


@pytest.mark.parametrize("P", [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 64])
@pytest.mark.parametrize("Np", [1, 2, 3])
@pytest.mark.parametrize("seed", [12345, 54321, 10101])
def test_cyqlone_factor_solve_scalar(P, Np, seed):
    N = Np * P
    run_test_cyqlone_factor_solve(Params(P=P, N=N, seed=seed))
    if N > 1:
        run_test_cyqlone_factor_solve(Params(P=P, N=N - 1, seed=seed))
    if N > 2:
        run_test_cyqlone_factor_solve(Params(P=P, N=N - 2, seed=seed))
    if N > 3:
        run_test_cyqlone_factor_solve(Params(P=P, N=N - 3, seed=seed))


@pytest.mark.parametrize("P", [2, 4, 8, 16, 64])
@pytest.mark.parametrize("Np", [1, 2, 3])
@pytest.mark.parametrize("seed", [12345, 54321, 10101])
def test_cyqlone_factor_solve_simd4(P, Np, seed):
    N = 4 * Np * P
    solver = cyqlone.simd4.v2.CyqloneSolver
    run_test_cyqlone_factor_solve(Params(P=P, N=N, seed=seed, solver=solver))
    if N > 1:
        run_test_cyqlone_factor_solve(Params(P=P, N=N - 1, seed=seed, solver=solver))
    if N > 2:
        run_test_cyqlone_factor_solve(Params(P=P, N=N - 2, seed=seed, solver=solver))
    if N > 3:
        run_test_cyqlone_factor_solve(Params(P=P, N=N - 3, seed=seed, solver=solver))


if __name__ == "__main__":
    plot_on_failure = True
    run_test_cyqlone_factor_solve(
        Params(P=4, N=16, seed=12345, nx=5, nu=3, solver=cyqlone.simd4.v2.CyqloneSolver)
    )
