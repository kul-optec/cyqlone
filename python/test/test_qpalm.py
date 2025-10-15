from cyqlone import OCP, CyqloneBackendSettings, Settings, LinearOCPSparseQP, SolverStatus
from cyqlone.cyqlone.simd4 import QPALM_Cyqlone
import numpy as np
import numpy.linalg as la
import pytest
import scipy.sparse as spa


@pytest.mark.parametrize("seed", [12345, 54321, 10101])
def test_cyqlone_qpalm(seed):
    lP = 3
    P = 1 << lP
    N = P * 12
    nx, nu = 19, 11
    ny, ny_N = 5, 5
    rng = np.random.default_rng(seed=seed)

    AB = rng.uniform(-1, 1, (N, nx, nx + nu))
    CD = rng.uniform(-1, 1, (N, ny, nx + nu))
    # These rows are eliminated from D(0)
    if ny >= 5:
        CD[0, [2, 4], nx:] = 0
    CN = rng.uniform(-1, 1, (ny_N, nx))
    QRS = rng.uniform(-1, 1, (N, nx + nu, nx + nu))
    QRS = QRS @ np.transpose(QRS, [0, 2, 1])
    QRS += 1e-3 * np.broadcast_to(np.eye(nx + nu), (N, nx + nu, nx + nu))
    QN = rng.uniform(-1, 1, (nx, nx))
    QN = QN @ QN.T + 1e-3 * np.eye(nx)
    rhs_eq = rng.uniform(-1, 1, (N + 1) * nx)
    rhs_lb = rng.uniform(-10, 0, N * ny + ny_N)
    rhs_ub = rhs_lb + rng.uniform(0, 20, N * ny + ny_N)
    # Ensure that x(0) is feasible
    rhs_lb[[2, 4]] = -np.inf
    rhs_ub[[2, 4]] = +np.inf

    ocp = OCP(
        AB=np.asfortranarray(AB),
        CD=np.asfortranarray(CD),
        CN=np.asfortranarray(CN),
        QRS=np.asfortranarray(QRS),
        QN=np.asfortranarray(QN),
        qr=rng.uniform(-1, 1, N * (nx + nu) + nx),
        rhs_eq=rhs_eq,
        rhs_lb=rhs_lb,
        rhs_ub=rhs_ub,
    )
    backend_settings = CyqloneBackendSettings()
    backend_settings.log_processors = lP + 2
    backend_settings.print_residuals = True
    qpalm_settings = Settings()
    qpalm_settings.verbose = True
    qpalm_settings.tolerance = 1e-10
    qpalm_settings.dual_tolerance = 1e-10
    qpalm_settings.eq_constr_tolerance = 1e-10
    solver = QPALM_Cyqlone(ocp, backend_settings, qpalm_settings)
    solver()
    assert solver() == SolverStatus.Converged
    assert solver.stats is not None
    print(solver.stats.timings.total)

    qp = LinearOCPSparseQP(ocp)
    x = np.concatenate((ocp.x0, solver.solution))
    b_l = np.concatenate((ocp.rhs_eq, ocp.rhs_lb))
    b_u = np.concatenate((ocp.rhs_eq, ocp.rhs_ub))
    Ax = qp.A @ x
    if ny >= 5:
        ny0 = ny - 2  # TODO: automate
        y0 = solver.inequality_multipliers[:ny0]
        y0 = np.insert(y0, 2, 0)
        y0 = np.insert(y0, 4, 0)
    else:
        ny0 = ny
        y0 = solver.inequality_multipliers[:ny0]
    λ0 = (
        ocp.Q(0) @ ocp.x0
        + ocp.qr[:nx]
        + ocp.S(0).T @ solver.solution[:nu]
        + ocp.A(0).T @ solver.equality_multipliers[:nx]
        + ocp.C(0).T @ y0
    )
    y = np.concatenate(
        (-λ0, -np.asarray(solver.equality_multipliers), y0, solver.inequality_multipliers[ny0:])
    )

    import qpalm

    qp_qpalm = qpalm.Data(*qp.A.shape[::-1])
    qp_qpalm.A = qp.A
    qp_qpalm.Q = qp.Q.T
    qp_qpalm.q = ocp.qr
    qp_qpalm.bmin = b_l
    qp_qpalm.bmax = b_u
    settings_qpalm = qpalm.Settings()
    settings_qpalm.verbose = False
    settings_qpalm.eps_abs = 1e-10
    settings_qpalm.eps_rel = 0
    solver_qpalm = qpalm.Solver(qp_qpalm, settings_qpalm)
    solver_qpalm.solve()

    if len(x) <= 100:
        print("difference x =", x - solver_qpalm.solution.x)
    if len(y) <= 100:
        print("difference y =", y - solver_qpalm.solution.y)
    print("difference x =", la.norm(x - solver_qpalm.solution.x, np.inf))
    print("difference y =", la.norm(y - solver_qpalm.solution.y, np.inf))

    Q = spa.tril(qp.Q) + spa.tril(qp.Q, -1).T
    assert la.norm(Ax - np.clip(Ax, b_l, b_u), np.inf) < qpalm_settings.dual_tolerance
    assert la.norm(Q @ x + ocp.qr + qp.A.T @ y, np.inf) < 10 * qpalm_settings.tolerance
