import cyqlone
import numpy as np
import numpy.linalg as la
import pytest
import scipy.sparse as spa

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



np.set_printoptions(
    precision=17, suppress=False, linewidth=800, formatter={"float_kind": "{:+.17e}".format}
)


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
        rhs_eq=rng.uniform(-1, 1, (N + 1) * nx),
        rhs_lb=rng.uniform(-1, 1, N * ny + ny_N),
        rhs_ub=rng.uniform(-1, 1, N * ny + ny_N),
    )


@pytest.mark.parametrize("P", [2, 4, 8, 16])
@pytest.mark.parametrize("NP", [1, 2, 3, 6])
@pytest.mark.parametrize("seed", [12345, 54321, 10101])
def test_cyqlone(P: int, NP: int, seed: int):
    # Dimensions
    N = P * NP
    lP = (P - 1).bit_length()
    nx, nu = 3, 2
    ny, ny_N = 5, 5
    rng = np.random.default_rng(seed=seed)

    # Generate random OCP
    ocp = random_ocp(N, nx, nu, ny, ny_N, rng)
    cocp = cyqlone.CyqloneOCP(ocp)

    # Solve
    solver = cyqlone.scalar.CyqloneSolver(cocp, lP)
    Σ = np.zeros(solver.num_general_constraints)
    Σ_packed = solver.pack_constraints(Σ)
    rq = solver.initialize_gradient(cocp)
    b = solver.initialize_rhs(cocp)
    rhs = -solver.build_rhs(rq, b)
    solver.factor_solve(np.inf, Σ_packed, ux := -np.copy(rq), λ := np.copy(b))
    solver.solve_reverse_new(ux, λ)
    sol = solver.build_rhs(ux, λ)

    # Extract solution
    x0 = ocp.b(0)
    uxs = solver.unpack_variables(ux)
    nux = nu + nx
    u = np.array([uxs[j * nux : nu + j * nux] for j in range(N)])
    x = np.array([uxs[nu + j * nux : nux + j * nux] for j in range(N)])
    x = np.vstack([x0[None, :], x])
    λ = solver.unpack_dynamics(λ).reshape((N, nx))
    λ_init = ocp.q(0) + ocp.Q(0) @ x0 + ocp.S(0).T @ u[0] + ocp.A(0).T @ λ[0]
    λ = np.vstack([λ, λ_init[None, :]])

    # These variants of the KKT conditions match equation (5) in the paper
    r0 = ocp.A(0) @ x[0] + ocp.B(0) @ u[0] + ocp.b(1) - x[1]
    assert la.norm(r0, np.inf) < 1e-10
    gx0 = ocp.Q(0) @ x[0] + ocp.S(0).T @ u[0] + ocp.q(0) + ocp.A(0).T @ λ[0] - λ[-1]
    assert la.norm(gx0, np.inf) < 1e-10
    gu0 = ocp.R(0) @ u[0] + ocp.S(0) @ x[0] + ocp.r(0) + ocp.B(0).T @ λ[0]
    assert la.norm(gu0, np.inf) < 1e-10
    for j in range(1, N):
        gxj = ocp.Q(j) @ x[j] + ocp.S(j).T @ u[j] + ocp.q(j) + ocp.A(j).T @ λ[j] - λ[j - 1]
        assert la.norm(gxj, np.inf) < 1e-10
        guj = ocp.R(j) @ u[j] + ocp.S(j) @ x[j] + ocp.r(j) + ocp.B(j).T @ λ[j]
        assert la.norm(guj, np.inf) < 1e-10
        rj = ocp.A(j) @ x[j] + ocp.B(j) @ u[j] + ocp.b(j + 1) - x[j + 1]
        assert la.norm(rj, np.inf) < 1e-10
    gN = ocp.Q(N) @ x[N] + ocp.q(N) - λ[N - 1]
    # TODO: the mapping of Q(N) and q(N) is wrong if 2*lP > N
    # gN = x[N] - λ[N - 1]
    assert la.norm(gN, np.inf) < 1e-10

    # Check sparse KKT system
    K = solver.build_sparse(ocp, Σ)
    K = spa.csr_array(spa.tril(K) + spa.triu(K.T, 1))
    assert la.norm(K @ sol - rhs, np.inf) < 1e-10

    # import matplotlib.pyplot as plt
    # print(K @ sol)
    # print(sol)
    # print(rhs)
    # plt.semilogy(np.abs(K @ sol - rhs), ".-")
    # plt.show()


if __name__ == "__main__":
    test_cyqlone(4, 5, 12345)
