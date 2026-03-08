#include <gtest/gtest.h>

#include <guanaqo/eigen/span.hpp>
#include <guanaqo/io/csv.hpp>
#include <guanaqo/print.hpp>
#include <vector>
using guanaqo::as_span;

#include <cyqlone/conversion.hpp>
#include <cyqlone/qpalm/example-problems/csv.hpp>
#include <cyqlone/qpalm/example-problems/platooning.hpp>
using cyqlone::index_t;
using cyqlone::real_t;
namespace qp = cyqlone::qpalm;

#include <qpalm.hpp>

#include <fstream>

TEST(QPALMBen, ocp) {
    // auto ocp = qp::problems::platooning({.Ts = 1.2 * 8, .N_horiz = 15});
    auto ocp  = qp::problems::platooning({.Ts = 1.2, .N_horiz = 127});
    auto grad = qp::reference_to_gradient(ocp.ocp, ocp.ref);

    // Quadratic program
    auto oqp          = cyqlone::LinearOCPSparseQP::build(ocp.ocp);
    qpalm::Data data  = {oqp.n, oqp.m_eq + oqp.m_ineq};
    auto to_eigen_idx = [](const auto v) {
        using std::begin;
        using std::end;
        return std::vector<Eigen::Index>{begin(v), end(v)};
    };
    auto Q_inner_idx = to_eigen_idx(oqp.Q_sparsity.inner_idx),
         Q_outer_ptr = to_eigen_idx(oqp.Q_sparsity.outer_ptr);
    data.set_Q(qpalm::sparse_mat_view_t{
        static_cast<Eigen::Index>(oqp.Q_sparsity.rows),
        static_cast<Eigen::Index>(oqp.Q_sparsity.cols),
        static_cast<Eigen::Index>(oqp.Q_sparsity.nnz()),
        Q_outer_ptr.data(),
        Q_inner_idx.data(),
        oqp.Q_values.data(),
    });
    auto A_inner_idx = to_eigen_idx(oqp.A_sparsity.inner_idx),
         A_outer_ptr = to_eigen_idx(oqp.A_sparsity.outer_ptr);
    data.set_A(qpalm::sparse_mat_view_t{
        static_cast<Eigen::Index>(oqp.A_sparsity.rows),
        static_cast<Eigen::Index>(oqp.A_sparsity.cols),
        static_cast<Eigen::Index>(oqp.A_sparsity.nnz()),
        A_outer_ptr.data(),
        A_inner_idx.data(),
        oqp.A_values.data(),
    });
    data.c = 0;
    data.q = qpalm::const_borrowed_vec_t{grad.data(), static_cast<Eigen::Index>(grad.size())};
    data.bmin.topRows(oqp.m_eq) = qpalm::const_borrowed_vec_t{
        ocp.rhs_eq.data(), static_cast<Eigen::Index>(ocp.rhs_eq.size())};
    data.bmax.topRows(oqp.m_eq) = qpalm::const_borrowed_vec_t{
        ocp.rhs_eq.data(), static_cast<Eigen::Index>(ocp.rhs_eq.size())};
    data.bmin.bottomRows(oqp.m_ineq) = qpalm::const_borrowed_vec_t{
        ocp.rhs_ineq_lb.data(), static_cast<Eigen::Index>(ocp.rhs_ineq_lb.size())};
    data.bmax.bottomRows(oqp.m_ineq) = qpalm::const_borrowed_vec_t{
        ocp.rhs_ineq_ub.data(), static_cast<Eigen::Index>(ocp.rhs_ineq_ub.size())};

    {
        std::ofstream f("platooning-A-sparse.csv");
        std::array A_shape{oqp.A_sparsity.rows, oqp.A_sparsity.cols};
        guanaqo::print_csv(f, std::span{A_shape});
        guanaqo::print_csv(f, std::span{oqp.A_values});
        guanaqo::print_csv(f, std::span{oqp.A_sparsity.inner_idx});
        guanaqo::print_csv(f, std::span{oqp.A_sparsity.outer_ptr});
    }
    {
        std::ofstream f("platooning-Q-sparse.csv");
        std::array Q_shape{oqp.Q_sparsity.rows, oqp.Q_sparsity.cols};
        guanaqo::print_csv(f, std::span{Q_shape});
        guanaqo::print_csv(f, std::span{oqp.Q_values});
        guanaqo::print_csv(f, std::span{oqp.Q_sparsity.inner_idx});
        guanaqo::print_csv(f, std::span{oqp.Q_sparsity.outer_ptr});
    }
    {
        std::ofstream f("platooning-eq.csv");
        guanaqo::print_csv(f, std::span{ocp.rhs_eq});
    }
    {
        std::ofstream f("platooning-lb.csv");
        guanaqo::print_csv(f, std::span{ocp.rhs_ineq_lb});
    }
    {
        std::ofstream f("platooning-ub.csv");
        guanaqo::print_csv(f, std::span{ocp.rhs_ineq_ub});
    }
    {
        std::ofstream f("platooning-grad.csv");
        guanaqo::print_csv(f, std::span{grad});
    }

    // Configure the solver
    // --------------------
    qpalm::Settings settings;
    settings.eps_abs     = 1e-7;
    settings.eps_rel     = 0;
    settings.max_iter    = 5000;
    qpalm::Solver solver = {data, settings};

    // Solve the roblem
    // ----------------
    solver.solve();
    auto sol  = solver.get_solution();
    auto info = solver.get_info();

    // Print the results
    // -----------------
    std::cout << "Solver status: " << info.status << "\n"
              << "Iter:          " << info.iter << "\n"
              << "Iter Out:      " << info.iter_out << "\n";

#ifdef QPALM_TIMING
    std::cout << "Setup time:    " << info.setup_time << "\n"
              << "Solve time:    " << info.solve_time << "\n"
              << "Run time:      " << info.run_time << "\n\n";
#endif

    {
        std::ofstream f("platooning-qpalm-sol-x.csv");
        guanaqo::print_csv(f, as_span(sol.x));
    }
    {
        std::ofstream f("platooning-qpalm-sol-y.csv");
        guanaqo::print_csv(f, as_span(sol.y));
    }
}
