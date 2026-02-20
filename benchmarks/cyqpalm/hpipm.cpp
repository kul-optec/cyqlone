#include "hpipm.hpp"

#include <cyqlone/ocp.hpp>
#include <batmat/assume.hpp>
#include <guanaqo/mat-view.hpp>
#include <chrono>
#include <cstddef>
#include <memory>
#include <numeric>

#include <hpipm_common.h>
#include <hpipm_d_ocp_qp.h>
#include <hpipm_d_ocp_qp_dim.h>
#include <hpipm_d_ocp_qp_ipm.h>
#include <hpipm_d_ocp_qp_sol.h>

using cyqlone::index_t;
using cyqlone::real_t;

struct HPIPMQPData {
    std::unique_ptr<std::byte[]> dim_mem;
    std::unique_ptr<struct d_ocp_qp_dim> dim;
    std::unique_ptr<std::byte[]> qp_mem;
    std::unique_ptr<struct d_ocp_qp> qp;
};

struct HPIPMSolverData {
    std::shared_ptr<HPIPMQPData> qp_data;
    std::unique_ptr<std::byte[]> qp_sol_mem;
    std::unique_ptr<struct d_ocp_qp_sol> qp_sol;
    std::unique_ptr<std::byte[]> solver_arg_mem;
    std::unique_ptr<struct d_ocp_qp_ipm_arg> arg;
    std::unique_ptr<std::byte[]> solver_mem;
    std::unique_ptr<struct d_ocp_qp_ipm_ws> workspace;
};

/// Create a column-major copy of a matrix, with the outer stride equal to the number of rows.
template <class T, class I, class S, guanaqo::StorageOrder O>
static std::vector<real_t> compress_col_major(guanaqo::MatrixView<T, I, S, O> mat) {
    std::vector<real_t> data(mat.rows * mat.cols);
    for (index_t j = 0; j < mat.cols; ++j)
        for (index_t i = 0; i < mat.rows; ++i)
            data[i + j * mat.rows] = mat(i, j);
    return data;
}

/// Create an n×n identity matrix.
static std::vector<real_t> eye(index_t n) {
    std::vector<real_t> data(n * n, real_t{0});
    for (index_t i = 0; i < n; ++i)
        data[i + i * n] = real_t{1};
    return data;
}

std::shared_ptr<HPIPMQPData> build_hpipm_qp(const cyqlone::LinearOCPStorage &ocp) {
    HPIPMQPData d;

    // Dimensions
    const int N = static_cast<int>(ocp.dim.N_horiz), nx = static_cast<int>(ocp.dim.nx),
              nu = static_cast<int>(ocp.dim.nu), ny = static_cast<int>(ocp.dim.ny),
              ny_N = static_cast<int>(ocp.dim.ny_N);
    d.dim_mem      = std::make_unique<std::byte[]>(d_ocp_qp_dim_memsize(N));
    d.dim          = std::make_unique<struct d_ocp_qp_dim>();
    d_ocp_qp_dim_create(N, d.dim.get(), d.dim_mem.get());

    for (int j = 0; j <= N; j++) {
        d_ocp_qp_dim_set_nx(j, nx, d.dim.get());
        if (j < N)
            d_ocp_qp_dim_set_nu(j, nu, d.dim.get());
        d_ocp_qp_dim_set_ng(j, j < N ? ny : ny_N, d.dim.get());
    }
    d_ocp_qp_dim_set_nbxe(0, nx, d.dim.get()); // initial state
    d_ocp_qp_dim_set_nbx(0, nx, d.dim.get());  // (equality requires inequality)

    // QP data
    d.qp_mem = std::make_unique<std::byte[]>(d_ocp_qp_memsize(d.dim.get()));
    d.qp     = std::make_unique<struct d_ocp_qp>();
    d_ocp_qp_create(d.dim.get(), d.qp.get(), d.qp_mem.get());
    d_ocp_qp_set_Jbx(0, eye(ocp.dim.nx).data(), d.qp.get());
    d_ocp_qp_set_lbx(0, const_cast<real_t *>(ocp.b(0).data), d.qp.get());
    d_ocp_qp_set_ubx(0, const_cast<real_t *>(ocp.b(0).data), d.qp.get());
    for (int j = 0; j < N; j++) {
        d_ocp_qp_set_A(j, compress_col_major(ocp.A(j)).data(), d.qp.get());
        d_ocp_qp_set_B(j, compress_col_major(ocp.B(j)).data(), d.qp.get());
        d_ocp_qp_set_b(j, const_cast<real_t *>(ocp.b(j + 1).data), d.qp.get());
        d_ocp_qp_set_C(j, compress_col_major(ocp.C(j)).data(), d.qp.get());
        d_ocp_qp_set_D(j, compress_col_major(ocp.D(j)).data(), d.qp.get());
        d_ocp_qp_set_lg(j, const_cast<real_t *>(ocp.b_min(j).data), d.qp.get());
        d_ocp_qp_set_ug(j, const_cast<real_t *>(ocp.b_max(j).data), d.qp.get());
        d_ocp_qp_set_Q(j, compress_col_major(ocp.Q(j)).data(), d.qp.get());
        d_ocp_qp_set_R(j, compress_col_major(ocp.R(j)).data(), d.qp.get());
        d_ocp_qp_set_S(j, compress_col_major(ocp.S(j)).data(), d.qp.get());
        d_ocp_qp_set_q(j, const_cast<real_t *>(ocp.q(j).data), d.qp.get());
        d_ocp_qp_set_r(j, const_cast<real_t *>(ocp.r(j).data), d.qp.get());
    }
    d_ocp_qp_set_Q(N, compress_col_major(ocp.Q(N)).data(), d.qp.get());
    d_ocp_qp_set_q(N, const_cast<real_t *>(ocp.q(N).data), d.qp.get());
    d_ocp_qp_set_C(N, compress_col_major(ocp.C(N)).data(), d.qp.get());
    d_ocp_qp_set_lg(N, const_cast<real_t *>(ocp.b_min(N).data), d.qp.get());
    d_ocp_qp_set_ug(N, const_cast<real_t *>(ocp.b_max(N).data), d.qp.get());

    return std::make_shared<HPIPMQPData>(std::move(d));
}

std::shared_ptr<HPIPMSolverData> create_hpipm_solver(std::shared_ptr<HPIPMQPData> qp_data,
                                                     int warm_start) {
    BATMAT_ASSERT(qp_data);
    HPIPMSolverData d;

    // Solution
    d.qp_sol_mem = std::make_unique<std::byte[]>(d_ocp_qp_sol_memsize(qp_data->dim.get()));
    d.qp_sol     = std::make_unique<struct d_ocp_qp_sol>();
    d_ocp_qp_sol_create(qp_data->dim.get(), d.qp_sol.get(), d.qp_sol_mem.get());

    // Solver arguments
    d.solver_arg_mem = std::make_unique<std::byte[]>(d_ocp_qp_ipm_arg_memsize(qp_data->dim.get()));
    d.arg            = std::make_unique<struct d_ocp_qp_ipm_arg>();
    d_ocp_qp_ipm_arg_create(qp_data->dim.get(), d.arg.get(), d.solver_arg_mem.get());

    int iter_max    = 100;
    double tol_stat = 1e-8;
    double tol_eq   = 1e-8;
    double tol_ineq = 1e-8;
    double tol_comp = 1e-5;
    d_ocp_qp_ipm_arg_set_default(hpipm_mode::SPEED, d.arg.get());
    d_ocp_qp_ipm_arg_set_iter_max(&iter_max, d.arg.get());
    d_ocp_qp_ipm_arg_set_tol_stat(&tol_stat, d.arg.get());
    d_ocp_qp_ipm_arg_set_tol_eq(&tol_eq, d.arg.get());
    d_ocp_qp_ipm_arg_set_tol_ineq(&tol_ineq, d.arg.get());
    d_ocp_qp_ipm_arg_set_tol_comp(&tol_comp, d.arg.get());
    d_ocp_qp_ipm_arg_set_warm_start(&warm_start, d.arg.get());

    // Solver workspace
    d.solver_mem =
        std::make_unique<std::byte[]>(d_ocp_qp_ipm_ws_memsize(qp_data->dim.get(), d.arg.get()));
    d.workspace = std::make_unique<struct d_ocp_qp_ipm_ws>();
    d_ocp_qp_ipm_ws_create(qp_data->dim.get(), d.arg.get(), d.workspace.get(), d.solver_mem.get());

    d.qp_data = std::move(qp_data);
    return std::make_shared<HPIPMSolverData>(std::move(d));
}

std::chrono::nanoseconds solve_hpipm(const HPIPMQPData &qp_data, HPIPMSolverData &solver_data) {
    auto t0 = std::chrono::steady_clock::now();
    d_ocp_qp_ipm_solve(qp_data.qp.get(), solver_data.qp_sol.get(), solver_data.arg.get(),
                       solver_data.workspace.get());
    auto t1 = std::chrono::steady_clock::now();
    return duration_cast<std::chrono::nanoseconds>(t1 - t0);
}

HPIPMStats get_stats_hpipm(HPIPMSolverData &solver_data) {
    HPIPMStats stats;
    d_ocp_qp_ipm_get_status(solver_data.workspace.get(), &stats.status);
    d_ocp_qp_ipm_get_iter(solver_data.workspace.get(), &stats.iter);
    d_ocp_qp_ipm_get_max_res_stat(solver_data.workspace.get(), &stats.max_res_stat);
    d_ocp_qp_ipm_get_max_res_eq(solver_data.workspace.get(), &stats.max_res_eq);
    d_ocp_qp_ipm_get_max_res_ineq(solver_data.workspace.get(), &stats.max_res_ineq);
    d_ocp_qp_ipm_get_max_res_comp(solver_data.workspace.get(), &stats.max_res_comp);
    return stats;
}

void warm_start_hpipm(HPIPMSolverData &solver_data, std::span<const real_t> solution) {
    auto &dim     = solver_data.qp_data->dim;
    auto nx_total = std::accumulate(dim->nx, dim->nx + dim->N + 1, 0);
    auto nu_total = std::accumulate(dim->nu, dim->nu + dim->N, 0);
    BATMAT_ASSERT(solution.size() == static_cast<size_t>(nx_total + nu_total));
    const auto *xu = solution.data();
    for (int j = 0; j < dim->N; ++j) {
        d_ocp_qp_sol_set_x(j, const_cast<real_t *>(xu), solver_data.qp_sol.get());
        xu += dim->nx[j];
        d_ocp_qp_sol_set_u(j, const_cast<real_t *>(xu), solver_data.qp_sol.get());
        xu += dim->nu[j];
    }
    d_ocp_qp_sol_set_x(dim->N, const_cast<real_t *>(xu), solver_data.qp_sol.get());
}

void update_x0_hpipm(cyqlone::LinearOCPStorage &ocp, HPIPMQPData &qp_data,
                     std::span<const cyqlone::real_t> solution) {
    auto &dim      = qp_data.dim;
    const auto nx0 = dim->nx[0], nu0 = dim->nu[0], nx1 = dim->nx[1];
    BATMAT_ASSERT(solution.size() >= static_cast<size_t>(nx0 + nu0 + nx1));
    const auto *x1 = solution.data() + nx0 + nu0;
    d_ocp_qp_set_lbx(0, const_cast<real_t *>(x1), qp_data.qp.get());
    d_ocp_qp_set_ubx(0, const_cast<real_t *>(x1), qp_data.qp.get());
    std::ranges::copy_n(x1, nx1, ocp.b(0).data);
}

void shift_solution_hpipm(HPIPMQPData &qp_data, std::span<cyqlone::real_t> solution) {
    const std::ptrdiff_t nx = qp_data.dim->nx[0], nu = qp_data.dim->nu[0];
    for (int j = 1; j < qp_data.dim->N; ++j) {
        BATMAT_ASSERT(qp_data.dim->nx[j] == nx);
        BATMAT_ASSERT(qp_data.dim->nu[j] == nu);
        std::ranges::copy(solution.subspan((nx + nu) * j, nx + nu),
                          solution.data() + (nx + nu) * (j - 1));
    }
    // Last state
    BATMAT_ASSERT(qp_data.dim->nx[qp_data.dim->N] == nx);
    std::ranges::copy(solution.subspan((nx + nu) * qp_data.dim->N, nx),
                      solution.data() + (nx + nu) * (qp_data.dim->N - 1));
}

cyqlone::LinearOCPStorage::Solution get_solution_hpipm(HPIPMSolverData &solver_data) {
    using std::abs;
    cyqlone::LinearOCPStorage::Solution sol;
    auto &dim     = solver_data.qp_data->dim;
    auto nx_total = std::accumulate(dim->nx, dim->nx + dim->N + 1, 0);
    auto nu_total = std::accumulate(dim->nu, dim->nu + dim->N, 0);
    sol.solution.resize(nx_total + nu_total);
    auto *xu = sol.solution.data();
    for (int j = 0; j < dim->N; ++j) {
        d_ocp_qp_sol_get_x(j, solver_data.qp_sol.get(), xu);
        xu += dim->nx[j];
        d_ocp_qp_sol_get_u(j, solver_data.qp_sol.get(), xu);
        xu += dim->nu[j];
    }
    d_ocp_qp_sol_get_x(dim->N, solver_data.qp_sol.get(), xu);

    auto ng_total = std::accumulate(dim->ng, dim->ng + dim->N + 1, 0);
    auto ng_max   = *std::max_element(dim->ng, dim->ng + dim->N + 1);
    sol.inequality_multipliers.resize(ng_total);
    std::vector<real_t> y_tmp(ng_max);
    auto *y_ug = sol.inequality_multipliers.data();
    for (int j = 0; j <= dim->N; ++j) {
        d_ocp_qp_sol_get_lam_ug(j, solver_data.qp_sol.get(), y_ug);
        d_ocp_qp_sol_get_lam_lg(j, solver_data.qp_sol.get(), y_tmp.data());
        for (int i = 0; i < dim->ng[j]; ++i)
            y_ug[i] -= y_tmp[i];
        y_ug += dim->ng[j];
    }

    sol.equality_multipliers.resize(nx_total);
    auto *y_eq = sol.equality_multipliers.data();
    std::vector<real_t> y_eq_tmp(dim->nx[0]);
    d_ocp_qp_sol_get_lam_lbx(0, solver_data.qp_sol.get(), y_eq);
    d_ocp_qp_sol_get_lam_ubx(0, solver_data.qp_sol.get(), y_eq_tmp.data());
    for (int i = 0; i < dim->nx[0]; ++i)
        y_eq[i] -= y_eq_tmp[i];
    y_eq += dim->nx[0];
    for (int j = 0; j < dim->N; ++j) {
        d_ocp_qp_sol_get_pi(j, solver_data.qp_sol.get(), y_eq);
        y_eq += dim->nx[j + 1];
    }
    return sol;
}
