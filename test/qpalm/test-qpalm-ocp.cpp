#include <gtest/gtest.h>

#include <batmat/loop.hpp>
#include <experimental/simd>
#include <guanaqo/io/csv.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/trace.hpp>
#include <fstream>

#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/example-problems/csv.hpp>
#include <cyqlone/qpalm/example-problems/platooning.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <cyqlone-version.h>

#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/qpalm/example-problems/conversion.hpp>
#if CYQLONE_WITH_MATIO
#include <cyqlone/matio.hpp>
#endif

using cyqlone::index_t;
using cyqlone::real_t;
namespace qp = cyqlone::qpalm;

#if GUANAQO_WITH_TRACING
static void init_trace() {
    guanaqo::trace_logger.reset();
    guanaqo::trace_logger.logs.resize(65565);
    GUANAQO_TRACE("init", 0);
}
static std::filesystem::path save_trace(const char *name) {
    std::optional tr = guanaqo::trace_logger.trace("end", 0);
    std::filesystem::path out_dir{"traces"};
    out_dir /= *cyqlone_commit_hash ? cyqlone_commit_hash : "unknown";
    std::filesystem::path out_file = out_dir / name;
    std::filesystem::create_directories(out_dir);
    std::ofstream csv{out_file};
    guanaqo::TraceLogger::write_column_headings(csv) << '\n';
    tr.reset();
    for (const auto &log : guanaqo::trace_logger.get_logs())
        csv << log << '\n';
    return out_file;
}
#endif

TEST(QPALM, cyqlone) {
    auto ocp =
        qp::problems::platooning({.N_horiz = 128, .masses{100, 150, 130, 70, 180, 170, 169, 130}});
    auto cocp          = cyqlone::CyqloneStorage<>::build(ocp.ocp);
    const bool verbose = true;
    auto &&backend     = qp::make_qpalm_cyqlone_backend<4>(cocp, {},
                                                           {.log_processors  = 5,
                                                            .print_residuals = verbose,
                                                            .print_precision = 17,
                                                            .pcg_print_resid = verbose});
    qp::Solver<qp::CyqloneBackend<4> *> qpalm{
        backend.get(),
        {.max_outer_iter                 = 400,
         .max_total_inner_iter           = 400,
         .tolerance                      = 1e-8,
         .dual_tolerance                 = 1e-8,
         .max_penalty_y                  = 1e7,
         .initial_penalty_y              = 1e-2,
         .verbose                        = verbose,
         .linesearch_include_multipliers = true},
    };

#if GUANAQO_WITH_TRACING
    for (index_t i = 0; !verbose && i < 50; ++i)
        qpalm(); // warm up
    init_trace();
#endif

    auto status = qpalm();
    EXPECT_EQ(status, qp::SolverStatus::Converged);

#if GUANAQO_WITH_TRACING
    std::cout << save_trace("test-QPALM-cyqlone.csv") << "\n\n";
#endif

    std::cout << "inner:   " << qpalm.stats->inner_iter << "\n"
              << "outer:   " << qpalm.stats->outer_iter << "\n"
              << "timings: " << qpalm.stats->timings.total << "\n";

    std::ofstream solution{"solution-platooning.csv"};
    auto x = qpalm.get_solution(), λ = qpalm.get_equality_multipliers(),
         y = qpalm.get_inequality_multipliers();
    guanaqo::print_csv(solution, std::span{x});
    guanaqo::print_csv(solution, std::span{λ});
    guanaqo::print_csv(solution, std::span{y});

#if CYQLONE_WITH_MATIO
    cyqlone::ocp_dump_mat("cyqlone-platooning.mat", ocp.ocp);
#endif
}

TEST(QPALM, cyqloneSpringsMasses) try {
    auto ocp  = qp::problems::load_from_csv("test/data/springs-masses", "masses=20-horiz=120");
    auto cocp = cyqlone::CyqloneStorage<>::build(ocp);
    const bool verbose = false;
    auto &&backend     = qp::make_qpalm_cyqlone_backend<4>(
        cocp, {}, {.log_processors = 4, .print_residuals = verbose, .pcg_print_resid = verbose});
    qp::Solver<qp::CyqloneBackend<4> *> qpalm{
        backend.get(),
        {.max_outer_iter = 500, .max_total_inner_iter = 1000, .verbose = verbose},
    };

#if GUANAQO_WITH_TRACING
    for (index_t i = 0; !verbose && i < 50; ++i)
        qpalm(); // warm up
    init_trace();
#endif

    auto status = qpalm();
    EXPECT_EQ(status, qp::SolverStatus::Converged);

#if GUANAQO_WITH_TRACING
    std::cout << save_trace("test-QPALM-cyqlone-spring-masses.csv") << "\n\n";
#endif

    std::cout << "inner:   " << qpalm.stats->inner_iter << "\n"
              << "outer:   " << qpalm.stats->outer_iter << "\n"
              << "timings: " << qpalm.stats->timings.total << "\n";

    std::ofstream solution{"solution-springs-masses.csv"};
    auto x = qpalm.get_solution(), λ = qpalm.get_equality_multipliers(),
         y = qpalm.get_inequality_multipliers();
    guanaqo::print_csv(solution, std::span{x});
    guanaqo::print_csv(solution, std::span{λ});
    guanaqo::print_csv(solution, std::span{y});

#if CYQLONE_WITH_MATIO
    cyqlone::ocp_dump_mat("cyqlone-spring-masses.mat", ocp);
#endif
} catch (guanaqo::io::csv_read_error &e) {
    GTEST_SKIP() << e.what();
}

#if 0 // TODO: support elimination of x0 using LinearOCPSparseQP
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.tpp>

#include <guanaqo/eigen/span.hpp>
#include <guanaqo/eigen/view.hpp>
#include <guanaqo/linalg/sparsity-conversions.hpp>

#include <Eigen/Cholesky>
#include <random>

using guanaqo::as_eigen;
using guanaqo::as_span;
using guanaqo::as_view;
using RealMatrixView = guanaqo::MatrixView<const real_t, index_t>;
namespace sp         = guanaqo::linalg::sparsity;
using namespace std::chrono_literals;

using mat = Eigen::MatrixX<real_t>;
using vec = Eigen::VectorX<real_t>;

TEST(QPALM, ocpBackend) {
    std::mt19937 rng{12345};
    std::normal_distribution<real_t> nrml{0, 1};

    // auto ocp = qp::problems::platooning({.Ts = 0.3 * 8, .N_horiz = 64});
    auto ocp       = qp::problems::platooning({.Ts = 0.3 * 32, .N_horiz = 16});
    auto q_grad    = qp::reference_to_gradient(ocp.ocp, ocp.ref);
    auto cocp      = cyqlone::CyqloneStorage<>::build(ocp.ocp, q_grad, ocp.rhs_eq, ocp.rhs_ineq_lb,
                                                      ocp.rhs_ineq_ub);
    auto &&backend = qp::make_qpalm_cyqlone_backend<4>(
        cocp, {}, {.log_processors = 3, .print_residuals = true, .pcg_print_resid = true});

    // Build quadratic program
    auto qp = qp::LinearOCPSparseQP::build(ocp.ocp);
    // Convert to dense matrices to compare with Eigen
    sp::SparsityConverter<sp::Sparsity, sp::Dense> conv_Q{qp.Q_sparsity};
    auto qp_Q_values = conv_Q.convert_values_copy(std::span{qp.Q_values});
    RealMatrixView qp_Q{{
        .data = qp_Q_values.data(),
        .rows = static_cast<index_t>(conv_Q.get_sparsity().rows),
        .cols = static_cast<index_t>(conv_Q.get_sparsity().cols),
    }};
    sp::SparsityConverter<sp::Sparsity, sp::Dense> conv_A{qp.A_sparsity};
    auto qp_A_values = conv_A.convert_values_copy(std::span{qp.A_values});
    RealMatrixView qp_A{{
        .data = qp_A_values.data(),
        .rows = static_cast<index_t>(conv_A.get_sparsity().rows),
        .cols = static_cast<index_t>(conv_A.get_sparsity().cols),
    }};
    auto [N, nx, nu, ny, ny_N] = ocp.ocp.dim;
    index_t n_var = N * (nx + nu) + nx, n_constr = N * ny + ny_N;
    index_t n_dyn_constr = (N + 1) * nx;

    auto A_ineq = qp_A.bottom_rows(n_constr);
    auto A_eq   = qp_A.top_rows(n_dyn_constr);

    auto x_strided = backend->var_vec();
    auto y_strided = backend->ineq_constr_vec();
    auto λ_strided = backend->eq_constr_vec();
    std::ranges::generate(x_strided, [&] { return nrml(rng); });
    std::ranges::generate(y_strided, [&] { return nrml(rng); });
    std::ranges::generate(λ_strided, [&] { return nrml(rng); });

    // Check gradient
    auto grad_strided = backend->var_vec();
    backend->grad_f(x_strided, grad_strided);
    vec x(backend->num_var()), grad(backend->num_var());
    backend->unscale_variables(x_strided, as_span(x));
    backend->unscale_variables(grad_strided, as_span(grad));
    vec grad_ref = as_eigen(qp_Q).selfadjointView<Eigen::Lower>() * x + as_eigen(std::span{q_grad});
    EXPECT_TRUE(grad.allFinite());
    EXPECT_LE((grad_ref - grad).lpNorm<Eigen::Infinity>(), 1e-10);

    // Check dynamics constraints Mx - b
    auto Mxb_strided = backend->eq_constr_resid(x_strided);
    vec Mxb(backend->num_eq_constr());
    backend->unscale_eq_constr(Mxb_strided, as_span(Mxb));
    vec Mxb_ref = as_eigen(A_eq) * x - as_eigen(std::span{ocp.rhs_eq});
    EXPECT_TRUE(Mxb.allFinite());
    EXPECT_LE((Mxb_ref - Mxb).lpNorm<Eigen::Infinity>(), 1e-10);

    auto Mᵀλ_strided = backend->mat_vec_MT(λ_strided);
    vec λ(backend->num_eq_constr()), Mᵀλ(backend->num_var());
    backend->unscale_eq_constr(λ_strided, as_span(λ));
    backend->unscale_variables(Mᵀλ_strided, as_span(Mᵀλ));
    vec Mᵀλ_ref = as_eigen(A_eq).transpose() * λ;
    EXPECT_TRUE(Mᵀλ.allFinite());
    EXPECT_LE((Mᵀλ_ref - Mᵀλ).lpNorm<Eigen::Infinity>(), 1e-10);

    // Check inequality constraints Ax
    auto Ax_strided = backend->mat_vec_A(x_strided);
    vec Ax(backend->num_ineq_constr());
    backend->unscale_ineq_constr(Ax_strided, as_span(Ax));
    vec Ax_ref = as_eigen(A_ineq) * x;
    EXPECT_TRUE(Ax.allFinite());
    EXPECT_LE((Ax_ref - Ax).lpNorm<Eigen::Infinity>(), 1e-10);

    // Check ALM quantities
    auto J_strided = backend->active_set();
    auto Σ_strided = backend->ineq_constr_vec();
    std::ranges::generate(Σ_strided, [&] { return std::exp2(nrml(rng)); });
    auto ŷ_strided   = backend->ineq_constr_vec();
    auto Aᵀŷ_strided = backend->var_vec();
    vec y(backend->num_ineq_constr()), Σ(backend->num_ineq_constr());
    backend->unscale_ineq_constr(y_strided, as_span(y));
    backend->unscale_ineq_constr(Σ_strided, as_span(Σ));
    backend->calc_ŷ_Aᵀŷ(Ax_strided, Σ_strided, y_strided, ŷ_strided, Aᵀŷ_strided, J_strided);
    vec ŷ(backend->num_ineq_constr());
    backend->unscale_ineq_constr(ŷ_strided, as_span(ŷ));
    vec Aᵀŷ(backend->num_var());
    backend->unscale_variables(Aᵀŷ_strided, as_span(Aᵀŷ));

    vec ζ_ref = Ax_ref + Σ.cwiseInverse().asDiagonal() * y;
    vec z_ref = ζ_ref.cwiseMax(as_eigen(std::span{ocp.rhs_ineq_lb}))
                    .cwiseMin(as_eigen(std::span{ocp.rhs_ineq_ub}));
    vec ŷ_ref = y + Σ.asDiagonal() * (Ax_ref - z_ref);
    EXPECT_TRUE(ŷ.allFinite());
    EXPECT_LE((ŷ_ref - ŷ).lpNorm<Eigen::Infinity>(), 1e-10);
    vec Aᵀŷ_ref = as_eigen(A_ineq).transpose() * ŷ;
    EXPECT_TRUE(Aᵀŷ.allFinite());
    EXPECT_LE((Aᵀŷ_ref - Aᵀŷ).lpNorm<Eigen::Infinity>(), 1e-10);

    vec err_ŷ = ŷ - ŷ_ref;
    guanaqo::print_python(std::cout << "ŷ:         \n", as_view(ŷ));
    guanaqo::print_python(std::cout << "ŷ eigen:   \n", as_view(ŷ_ref));
    guanaqo::print_python(std::cout << "difference:\n", as_view(err_ŷ));

    // Check KKT system solution
    real_t S = 10;

    auto d_strided = backend->var_vec(), ξ_strided = backend->var_vec(),
         MᵀΔλ_strided = backend->var_vec();
    auto Ad_strided   = backend->ineq_constr_vec();
    auto Δλ_strided   = backend->eq_constr_vec();
    backend->solve(x_strided, grad_strided, Mᵀλ_strided, Aᵀŷ_strided, Mxb_strided, S, Σ_strided,
                   J_strided, //
                   d_strided, ξ_strided, Ad_strided, Δλ_strided, MᵀΔλ_strided);

    vec d(backend->num_var()), ξ(backend->num_var()), MᵀΔλ(backend->num_var());
    backend->unscale_variables(d_strided, as_span(d));
    backend->unscale_variables(ξ_strided, as_span(ξ));
    backend->unscale_variables(MᵀΔλ_strided, as_span(MᵀΔλ));
    vec Ad(backend->num_ineq_constr()), Δλ(backend->num_eq_constr());
    backend->unscale_ineq_constr(Ad_strided, as_span(Ad));
    backend->unscale_eq_constr(Δλ_strided, as_span(Δλ));

    vec J(backend->num_ineq_constr());
    backend->unscale_ineq_constr(J_strided, as_span(J));
    mat H_schur = as_eigen(qp_Q) + 1 / S * mat::Identity(n_var, n_var) +
                  as_eigen(A_ineq).transpose() * J.select(Σ, 0).asDiagonal() * as_eigen(A_ineq);
    auto Hllt      = H_schur.selfadjointView<Eigen::Lower>().llt();
    real_t rcond_H = H_schur.selfadjointView<Eigen::Lower>().ldlt().rcond();
    vec g_eigen    = grad_ref + Mᵀλ_ref + Aᵀŷ_ref;
    vec v_eigen    = Hllt.solve(g_eigen);
    mat Ψ_half     = Hllt.matrixL().solve(as_eigen(A_eq).transpose());
    mat Ψ          = mat::Zero(n_dyn_constr, n_dyn_constr);
    Ψ.selfadjointView<Eigen::Lower>().rankUpdate(Ψ_half.transpose());
    auto Ψllt      = Ψ.selfadjointView<Eigen::Lower>().llt();
    real_t rcond_Ψ = Ψ.selfadjointView<Eigen::Lower>().ldlt().rcond();
    vec Δλ_ref     = Ψllt.solve(Mxb_ref - as_eigen(A_eq) * v_eigen);
    vec MᵀΔλ_ref   = as_eigen(A_eq).transpose() * Δλ_ref;
    vec d_ref      = Hllt.solve(-MᵀΔλ_ref - Mᵀλ_ref - grad_ref - Aᵀŷ_ref);
    vec Ad_ref     = as_eigen(A_ineq) * d_ref;
    vec ξ_ref      = (as_eigen(qp_Q) + 1 / S * mat::Identity(n_var, n_var)) * d_ref;

    const auto eps = 50 * std::numeric_limits<real_t>::epsilon();
    std::cout << "cond H: " << 1 / rcond_H << "\n";
    std::cout << "cond Ψ: " << 1 / rcond_Ψ << "\n";
    EXPECT_LE((d - d_ref).lpNorm<Eigen::Infinity>(), eps / rcond_H / rcond_Ψ);
    EXPECT_LE((ξ - ξ_ref).lpNorm<Eigen::Infinity>(), eps / rcond_H / rcond_Ψ);
    EXPECT_LE((MᵀΔλ - MᵀΔλ_ref).lpNorm<Eigen::Infinity>(), eps / rcond_H / rcond_Ψ);
    EXPECT_LE((Ad - Ad_ref).lpNorm<Eigen::Infinity>(), eps / rcond_H / rcond_Ψ);
    EXPECT_LE((Δλ - Δλ_ref).lpNorm<Eigen::Infinity>(), eps / rcond_H / rcond_Ψ);
}
#endif
