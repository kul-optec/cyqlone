#include <gtest/gtest.h>

#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>
#include <cyqlone/packing.hpp>
#include <cyqlone/random-ocp.hpp>
#include <cyqlone/tracing.hpp>
#include <batmat/linalg/simdify.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/trace.hpp>
#include <cyqlone-version.h>
#if CYQLONE_WITH_MATIO
#include <cyqlone/matio.hpp>
#endif

#if GUANAQO_WITH_TRACING
#include <filesystem>
#endif
#include <iostream>
#include <limits>

using cyqlone::index_t;
using cyqlone::real_t;

#define NO_PENALTY 0
#define WITH_UPDATES 1

class CyqloneFactorTest : public testing::TestWithParam<cyqlone::SolveMethod> {};

template <class M>
auto unpacked(const M &m) {
    using Mat = batmat::matrix::Matrix<typename M::value_type, typename M::index_type>;
    Mat res{{.depth = m.depth(), .rows = m.rows(), .cols = m.cols()}};
    for (index_t b = 0; b < m.num_batches(); ++b)
        cyqlone::linalg::unpack(m.batch(b), res.middle_batches(b * m.batch_size(), m.batch_size()));
    return res;
}

// Tests the factorization, factorization updates, and solution of the Cyqlone linear solver.
TEST_P(CyqloneFactorTest, factor) {
    using namespace cyqlone;
    using batmat::linalg::simdify;

    const index_t p  = 8;
    using Solver     = CyqloneSolver<4, real_t, StorageOrder::RowMajor>;
    const index_t ny = 50, ny_0 = 25, ny_N = 25, nyM = std::max(ny, ny_0 + ny_N);
    OCPDim dim{.N_horiz = 96, .nx = 7, .nu = 5, .ny = ny, .ny_N = ny_N};
    const index_t nux = dim.nu + dim.nx, N = dim.N_horiz;
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    std::mt19937 rng(102030405);
    std::uniform_real_distribution<real_t> uni(-1, 1);
    std::bernoulli_distribution bern(0.01);
    std::generate_n(ocp.qr().data, ocp.qr().rows, [&] { return uni(rng); });
    std::generate_n(ocp.b().data, ocp.b().rows, [&] { return uni(rng); });
    std::generate_n(ocp.b_min().data, ocp.b_min().rows, [&] { return uni(rng); });
    std::generate_n(ocp.b_max().data, ocp.b_max().rows, [&] { return uni(rng); });
    auto cocp                                 = CyqloneStorage<real_t>::build(ocp);
    Solver solver                             = Solver::build(cocp, p);
    auto tricyqle_params                      = solver.get_tricyqle_params();
    tricyqle_params.solve_method              = GetParam();
    tricyqle_params.cr_max_update_fraction_Y0 = 9999;
    tricyqle_params.pcr_max_update_fraction   = 9999;
    solver.update_tricyqle_params(tricyqle_params);

    // Spin a bit longer to get more deterministic timings
    solver.set_barrier_spin_count(std::numeric_limits<uint32_t>::max());

    GUANAQO_IF_ITT(solver.run(
        [](auto &ctx) { __itt_thread_set_name(std::format("OMP({})", ctx.index).c_str()); }));

    std::vector<real_t> Σ_lin((N - 1) * ny + ny_0 + ny_N);
    Solver::matrix b{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        λ{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        rq{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        ux{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        Mᵀλ{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        DCux{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        DCᵀΣDCux{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        grad{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        Mxb{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        Σ{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        Σ2{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        ΔΣ{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}};

    // Initialize penalties (two different ones, to test the updates)
    std::ranges::generate(Σ_lin, [&] { return std::exp2(uni(rng)); });
    std::vector<real_t> Σ_lin2 = Σ_lin;
#if NO_PENALTY
    std::ranges::fill(Σ_lin2, 0);
#elif WITH_UPDATES
    for (auto &Σ2i : Σ_lin2)
        if (bern(rng))
            Σ2i = std::exp2(uni(rng));
#endif
    solver.pack_constraints(Σ_lin, Σ);
    solver.pack_constraints(Σ_lin2, Σ2);
    std::ranges::transform(Σ2, Σ, std::ranges::begin(ΔΣ), std::minus<>{});

    // Initialize the right-hand side of the KKT system
    solver.initialize_gradient(cocp, rq);
    solver.initialize_rhs(cocp, b);

    // Repeatedly factor and solve the KKT system (as a warm-up run for the timing and to make sure
    // the workspaces are still okay after a couple of iterations)
    for (int i = 0; i < 50; ++i) {
        cyqlone::linalg::negate(rq, ux);
        λ = b;
        solver.run([&](auto &ctx) {
            solver.factor(ctx, 1e100, Σ);
#if WITH_UPDATES
            solver.update(ctx, ΔΣ);
#endif
            solver.solve_forward(ctx, ux, λ);
            solver.solve_reverse(ctx, ux, λ);
            solver.residual_dynamics_constr(ctx, ux, b, Mxb);
            solver.transposed_dynamics_constr(ctx, λ, Mᵀλ);
            solver.cost_gradient(ctx, ux, 1, rq, 0, grad);
            solver.general_constr(ctx, ux, DCux);
            ctx.run_single_sync([&] { cyqlone::linalg::hadamard(DCux, Σ2); });
            solver.transposed_general_constr(ctx, DCux, DCᵀΣDCux);
        });
    }

    // Now factor and solve the KKT system for real
    cyqlone::linalg::negate(rq, ux);
    λ = b;
#if GUANAQO_WITH_TRACING
    guanaqo::get_trace_logger().reset();
#endif
    solver.run([&](auto &ctx) {
        solver.factor(ctx, 1e100, Σ);
#if WITH_UPDATES
        solver.update(ctx, ΔΣ);
#endif
        solver.solve_forward(ctx, ux, λ);
        solver.solve_reverse(ctx, ux, λ);
        solver.residual_dynamics_constr(ctx, ux, b, Mxb);                  // Mz + b  (= 0)
        solver.transposed_dynamics_constr(ctx, λ, Mᵀλ);                    // Mᵀλ
        solver.cost_gradient(ctx, ux, 1, rq, 0, grad);                     // Qz + q
        solver.general_constr(ctx, ux, DCux);                              // Gz
        ctx.run_single_sync([&] { cyqlone::linalg::hadamard(DCux, Σ2); }); // ΣGz
        solver.transposed_general_constr(ctx, DCux, DCᵀΣDCux);             // GᵀΣGz
    });

    // Check the residuals
    using std::pow;
    const auto ε = pow(std::numeric_limits<real_t>::epsilon(), 0.6);
    cyqlone::linalg::axpy(grad, {1, 1}, DCᵀΣDCux, Mᵀλ); // gradient of the Lagrangian
    std::cout << "dynamics constraints: " << guanaqo::float_to_str(cyqlone::linalg::norm_inf(Mxb))
              << "\nstationarity:         "
              << guanaqo::float_to_str(cyqlone::linalg::norm_inf(grad)) << "\n";
    EXPECT_LE(cyqlone::linalg::norm_inf(Mxb), ε);
    EXPECT_LE(cyqlone::linalg::norm_inf(grad), ε);

#if CYQLONE_WITH_MATIO
    auto K   = solver.build_sparse(cocp, Σ_lin2);
    auto L   = solver.build_sparse_factor();
    auto D   = solver.build_sparse_diag();
    auto rhs = solver.build_rhs(rq, b);
    auto sol = solver.build_sol(ux, λ);

    // Export the problem, the Cholesky factor, and the solution as a .mat file
    std::filesystem::path filename = "test-cyqlone.mat";
    auto matfile                   = cyqlone::create_mat(filename);
    cyqlone::add_to_mat(matfile.get(), "ocp", ocp);
    cyqlone::add_to_mat(matfile.get(), "K", K);
    cyqlone::add_to_mat(matfile.get(), "L", L);
    cyqlone::add_to_mat(matfile.get(), "D", D);
    cyqlone::add_to_mat(matfile.get(), "rhs", std::span{rhs});
    cyqlone::add_to_mat(matfile.get(), "sol", std::span{sol});
    cyqlone::add_to_mat(matfile.get(), "sigma", unpacked(Σ2));
    cyqlone::add_to_mat(matfile.get(), "LH", unpacked(solver.riccati_LH));
    cyqlone::add_to_mat(matfile.get(), "LAB", unpacked(solver.riccati_LAB));
    // Execute the factorization with Σ2 directly (instead of factorizing with Σ and updating)
    solver.run([&](auto &ctx) { solver.factor(ctx, 1e100, Σ2); });
    cyqlone::add_to_mat(matfile.get(), "L_refactor", solver.build_sparse_factor());
    std::cout << filename << "\n";
#endif

#if GUANAQO_WITH_TRACING
    {
        solver.run([](auto &ctx) { GUANAQO_TRACE("thread_id", ctx.index); });
        std::string name = std::format("factor_cyclic_new.csv");
        std::filesystem::path out_dir{"traces"};
        out_dir /= *cyqlone_commit_hash ? cyqlone_commit_hash : "unknown";
        out_dir /= solver.get_params_string();
        std::filesystem::create_directories(out_dir);
        std::ofstream csv{out_dir / name};
        guanaqo::TraceLogger::write_column_headings(csv) << '\n';
        auto logs = guanaqo::get_trace_logger().get_logs();
        for (const auto &log : logs)
            csv << log << '\n';
        std::cout << (out_dir / name) << std::endl;
        write_chrome_trace((out_dir / name).replace_extension("json.gz"), logs);
        std::cout << (out_dir / name).replace_extension("json.gz") << std::endl;
    }
#endif
}

// The tests below check that the workspaces for the CR factorization updates don't overlap.
// In the case where p is a power of two, this is obvious, but for other values of p, we may need
// to tweak the assignment of the last block.
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 1  >>> <<<                          |  L₁                                // 1˃  [0, 1)    1˂  [1, 2)
// 3          >>> <<<                  |      L₃                            // 3˃  [2, 3)    3˂  [3, 4)
// 5                  >>> <<<          |          L₅                        // 5˃  [4, 5)    5˂  [5, 6)
// 7                          >>> <00  |              L₇                    // 7˃  [6, 7)    7˂  [7, 8)
// 2      >>> <<<                      |  Y₁  U₃          L₂                // 2˃  [1, 2)    2˂  [2, 3)
// 6                      >>> <<<      |          Y₅  U₇      L₆            // 6˃  [5, 6)    6˂  [6, 7)
// 4              >>> <<<              |      Y₃  U₅      Y₂  U₆  L₄        // 4˃  [3, 4)    4˂  [4, 5)
// 0  <<<                         0>>  |  U₁          00  U₂  00  U₄  L₀    // 0˃  [7, 8)    0˂  [0, 1)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 2  >>>>>>> <<<<<<<                  |  L₂                // 2˃  [0, 2)    2˂  [2, 4)
// 6                  >>>>>>> <<<<<00  |      L₆            // 6˃  [4, 6)    6˂  [6, 8)
// 4          >>>>>>> <<<<<<<          |  Y₂  U₆  L₄        // 4˃  [2, 4)    4˂  [4, 6)
// 0  <<<<<<<                 00000>>  |  U₂  00  U₄  L₀    // 0˃  [6, 8)    0˂  [0, 2)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 4  >>>>>>>>>>>>>>> <<<<<<<<<<<<<00  |  L₄        // 4˃  [0, 4)    4˂  [4, 8)
// 0  <<<<<<<<<<<<<<< 0000000000000>>  |  U₄  L₀    // 0˃  [4, 8)    0˂  [0, 4)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 0  <<<<<<<<<<<<<<< >>>>>>>>>>>>>>>  |  L₀    // 0˃  [4, 8)    0˂  [0, 4)

//    (0) (1) (2) (3) (4) (5) (6) (7)
// 1  >>> <<<                            // 1˃  [0, 1)    1˂  [1, 2)
// 3          >>> <<<                    // 3˃  [2, 3)    3˂  [3, 4)
// 5                  >>> <<<            // 5˃  [4, 5)    5˂  [5, 6)
// 7                          >>> <<<    // 7˃  [6, 7)    7˂  [7, 8)
// 2      >>> <<<                        // 2˃  [1, 2)    2˂  [2, 3)
// 6                      >>> <<<        // 6˃  [5, 6)    6˂  [6, 7)
// 4              >>> <<<                // 4˃  [3, 4)    4˂  [4, 5)
// 0  <<<                         >>>    // 0˃  [7, 8)    0˂  [0, 1)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 2  >>>>>>> <<<<<<<                    // 2˃  [0, 2)    2˂  [2, 4)
// 6                  >>>>>>> <<<<<<<    // 6˃  [4, 6)    6˂  [6, 8)
// 4          >>>>>>> <<<<<<<            // 4˃  [2, 4)    4˂  [4, 6)
// 0  <<<<<<<                 >>>>>>>    // 0˃  [6, 8)    0˂  [0, 2)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 4  >>>>>>>>>>>>>>> <<<<<<<<<<<<<<<    // 4˃  [0, 4)    4˂  [4, 8)
// 0  <<<<<<<<<<<<<<< >>>>>>>>>>>>>>>    // 0˃  [4, 8)    0˂  [0, 4)
TEST(CyqloneTest, updateIndices8) {
    using namespace cyqlone;

    const index_t p  = 8;
    using Solver     = CyqloneSolver<1, real_t, StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::v, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.tricyqle.m_update, index_t{1});

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 3), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 5), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 5), std::make_pair(5, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 7), std::make_pair(6, 7));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 7), std::make_pair(7, 8));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 6), std::make_pair(5, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 6), std::make_pair(6, 7));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 0), std::make_pair(7, 8));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 2), std::make_pair(2, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 6), std::make_pair(4, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 6), std::make_pair(6, 8));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 4), std::make_pair(4, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 0), std::make_pair(6, 8));
}

//    (0) (1) (2) (3) (4) (5) (6) (7)
// 1  >>> <<<                            // 1˃  [0, 1)    1˂  [1, 2)
// 3          >>> <<<                    // 3˃  [2, 3)    3˂  [3, 4)
// 5                  >>> <<<            // 5˃  [4, 5)    5˂  [5, 6)
// x
// 2      >>> <<<                        // 2˃  [1, 2)    2˂  [2, 3)
// 6                      >>> <<<        // 6˃  [5, 6)    6˂  [6, 7)
// 4              >>> <<<                // 4˃  [3, 4)    4˂  [4, 5)
// 0  <<<                     >>>        // 0˃  [6, 7)    0˂  [0, 1)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 2  >>>>>>> <<<<<<<                    // 2˃  [0, 2)    2˂  [2, 4)
// 6                  >>>>>>> <<<        // 6˃  [4, 6)    6˂  [6, 7)
// 4          >>>>>>> <<<<<<<            // 4˃  [2, 4)    4˂  [4, 6)
// 0  <<<<<<<                 >>>        // 0˃  [6, 7)    0˂  [0, 2)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 4  >>>>>>>>>>>>>>> <<<<<<<<<<<        // 4˃  [0, 4)    4˂  [4, 7)
// 0  <<<<<<<<<<<<<<< >>>>>>>>>>>        // 0˃  [4, 7)    0˂  [0, 4)
TEST(CyqloneTest, updateIndices7) {
    using namespace cyqlone;

    const index_t p  = 7;
    using Solver     = CyqloneSolver<1, real_t, StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::v, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.tricyqle.m_update, index_t{1});

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 3), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 5), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 5), std::make_pair(5, 6));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 6), std::make_pair(5, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 6), std::make_pair(6, 7));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 0), std::make_pair(6, 7));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 2), std::make_pair(2, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 6), std::make_pair(4, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 6), std::make_pair(6, 7));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 4), std::make_pair(4, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 0), std::make_pair(6, 7));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(2, 4), std::make_pair(0, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(2, 4), std::make_pair(4, 7));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(2, 0), std::make_pair(0, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(2, 0), std::make_pair(4, 7));
}

//    (0) (1) (2) (3) (4) (5) (6) (7)
// 1  >>> <<<                            // 1˃  [0, 1)    1˂  [1, 2)
// 3          >>> <<<                    // 3˃  [2, 3)    3˂  [3, 4)
// 5                  >>> <<<            // 5˃  [4, 5)    5˂  [5, 6)
// x
// 2      >>> <<<                        // 2˃  [1, 2)    2˂  [2, 3)
// x
// 4              >>> <<<                // 4˃  [3, 4)    4˂  [4, 5)
// 0  <<<                 >>>            // 0˃  [5, 6)    0˂  [0, 1)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 2  >>>>>>> <<<<<<<                    // 2˃  [0, 2)    2˂  [2, 4)
// x
// 4          >>>>>>> <<<<<<<            // 4˃  [2, 4)    4˂  [4, 6)
// 0  <<<<<<<         >>>>>>>            // 0˃  [4, 6)    0˂  [0, 2)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 4  >>>>>>>>>>>>>>> <<<<<<<            // 4˃  [0, 4)    4˂  [4, 6)
// 0  <<<<<<<<<<<<<<< >>>>>>>            // 0˃  [4, 6)    0˂  [0, 4)
TEST(CyqloneTest, updateIndices6) {
    using namespace cyqlone;

    const index_t p  = 6;
    using Solver     = CyqloneSolver<1, real_t, StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::v, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.tricyqle.m_update, index_t{1});

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 3), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 5), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 5), std::make_pair(5, 6));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 0), std::make_pair(5, 6));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 2), std::make_pair(2, 4));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 4), std::make_pair(4, 6));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 0), std::make_pair(4, 6));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(2, 4), std::make_pair(0, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(2, 4), std::make_pair(4, 6));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(2, 0), std::make_pair(0, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(2, 0), std::make_pair(4, 6));
}

//    (0) (1) (2) (3) (4) (5) (6) (7)
// 1  >>> <<<                            // 1˃  [0, 1)    1˂  [1, 2)
// 3          >>> <<<                    // 3˃  [2, 3)    3˂  [3, 4)
// x
// x
// 2      >>> <<<                        // 2˃  [1, 2)    2˂  [2, 3)
// x
// 4              >>> <<<                // 4˃  [3, 4)    4˂  [4, 5)
// 0  <<<             >>>                // 0˃  [4, 5)    0˂  [0, 1)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 2  >>>>>>> <<<<<<<                    // 2˃  [0, 2)    2˂  [2, 4)
// x
// 4          >>>>>>> <<<                // 4˃  [2, 4)    4˂  [4, 5)
// 0  <<<<<<<         >>>                // 0˃  [4, 5)    0˂  [0, 2)
//
//    (0) (1) (2) (3) (4) (5) (6) (7)
// 4  >>>>>>>>>>>>>>> <<<                // 4˃  [0, 4)    4˂  [4, 5)
// 0  <<<<<<<<<<<<<<< >>>                // 0˃  [4, 5)    0˂  [0, 4)
TEST(CyqloneTest, updateIndices5) {
    using namespace cyqlone;

    const index_t p  = 5;
    using Solver     = CyqloneSolver<1, real_t, StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::v, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.tricyqle.m_update, index_t{1});

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 3), std::make_pair(3, 4));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(0, 0), std::make_pair(4, 5));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 2), std::make_pair(2, 4));

    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(1, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(1, 0), std::make_pair(4, 5));

    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(2, 4), std::make_pair(0, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(2, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.tricyqle.cols_Ups_bwd(2, 0), std::make_pair(0, 4));
    EXPECT_EQ(solver.tricyqle.cols_Ups_fwd(2, 0), std::make_pair(4, 5));
}

INSTANTIATE_TEST_SUITE_P(CyqloneSolverConfigs, CyqloneFactorTest,
                         ::testing::Values(cyqlone::SolveMethod::StairPCG,
                                           cyqlone::SolveMethod::JacobiPCG,
                                           cyqlone::SolveMethod::PCR),
                         ([](const ::testing::TestParamInfo<CyqloneFactorTest::ParamType> &info) {
                             return enum_name(info.param);
                         }));
