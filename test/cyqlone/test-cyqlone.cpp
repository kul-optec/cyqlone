#include <gtest/gtest.h>

#include <cyqlone/random-ocp.hpp>
#include <cyqlone/v2/cyqlone.hpp>
#include <batmat/linalg/simdify.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/trace.hpp>
#include <cyqlone-version.h>

#if GUANAQO_WITH_TRACING
#include <filesystem>
#endif
#include <fstream>
#include <iostream>
#include <limits>

using cyqlone::index_t;
using cyqlone::real_t;

#define WITH_UPDATES 1

class CyqloneFactorTest : public testing::TestWithParam<cyqlone::SolveMethod> {};

TEST_P(CyqloneFactorTest, factor) {
    using namespace cyqlone;
    using batmat::linalg::simdify;

    using Solver     = v2::CyqloneSolver<4, real_t, v2::StorageOrder::RowMajor>;
    const index_t ny = 50, ny_0 = 25, ny_N = 25;
    OCPDim dim{.N_horiz = 97, .nx = 40, .nu = 30, .ny = ny, .ny_N = ny_N};
    const index_t nux = dim.nu + dim.nx, N = dim.N_horiz;
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny_0).set_constant(0);
    std::mt19937 rng(102030405);
    std::uniform_real_distribution<real_t> uni(-1, 1);
    std::bernoulli_distribution bern(0.01);
    std::generate_n(ocp.qr().data, ocp.qr().rows, [&] { return uni(rng); });
    std::generate_n(ocp.b().data, ocp.b().rows, [&] { return uni(rng); });
    std::generate_n(ocp.b_min().data, ocp.b_min().rows, [&] { return uni(rng); });
    std::generate_n(ocp.b_max().data, ocp.b_max().rows, [&] { return uni(rng); });
    auto cocp           = CyqloneStorage<real_t>::build(ocp);
    Solver solver       = Solver::build(cocp, 8);
    solver.solve_method = GetParam();

    // Spin a bit longer to get more deterministic timings
    solver.parallel_ctx->barrier.spin_count = std::numeric_limits<uint32_t>::max();

    GUANAQO_IF_ITT(solver.parallel_ctx->run(
        [](auto &ctx) { __itt_thread_set_name(std::format("OMP({})", ctx.index).c_str()); }));

    const index_t nyM = std::max(ny, ny_0 + ny_N);
    std::vector<real_t> Σ_lin((N - 1) * ny + ny_0 + ny_N);
    Solver::matrix λ{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        ux{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        Mᵀλ{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        DCux{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        DCᵀΣDCux{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        grad{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        Mxb{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        Σ{{.depth = solver.ceil_N(), .rows = dim.ny, .cols = 1}},
        Σ2{{.depth = solver.ceil_N(), .rows = dim.ny, .cols = 1}},
        ΔΣ{{.depth = solver.ceil_N(), .rows = dim.ny, .cols = 1}};
    std::ranges::generate(λ, [&] { return uni(rng); });
    std::ranges::generate(ux, [&] { return uni(rng); });
    std::ranges::generate(Σ_lin, [&] { return std::exp2(uni(rng)); });
    std::vector<real_t> Σ_lin2 = Σ_lin;
#if WITH_UPDATES
    for (auto &Σ2i : Σ_lin2)
        if (bern(rng))
            Σ2i = std::exp2(uni(rng));
#endif
    solver.pack_constraints(Σ_lin, Σ);
    solver.pack_constraints(Σ_lin2, Σ2);
    std::ranges::transform(Σ2, Σ, std::ranges::begin(ΔΣ), std::minus<>{});

    if (std::ofstream f("rhs.csv"); f) {
        auto b = solver.build_rhs(ux, λ);
        for (auto x : b)
            f << guanaqo::float_to_str(x) << '\n';
    }

    const auto ux_initial = ux, λ_initial = λ;
    for (int i = 0; i < 50; ++i) {
        solver.parallel_ctx->run([&](auto &ctx) {
            solver.factor(ctx, 1e100, Σ);
#if WITH_UPDATES
            solver.update(ctx, ΔΣ);
#endif
            solver.solve_forward(ctx, ux, λ);
            solver.solve_reverse(ctx, ux, λ);
            solver.residual_dynamics_constr(ctx, ux, λ_initial, Mxb);
            solver.transposed_dynamics_constr(ctx, λ, Mᵀλ);
            solver.cost_gradient(ctx, ux, -1, ux_initial, 0, grad);
            solver.general_constr(ctx, ux, DCux);
            ctx.arrive_and_wait();
            if (ctx.is_master()) // TODO
                Solver::compact_blas::xhadamard(simdify(Σ2), simdify(DCux));
            ctx.arrive_and_wait();
            solver.transposed_general_constr(ctx, DCux, DCᵀΣDCux);
        });
        ux.view() = ux_initial.view();
        λ.view()  = λ_initial.view();
    }
#if GUANAQO_WITH_TRACING
    guanaqo::get_trace_logger().reset();
#endif
    solver.parallel_ctx->run([&](auto &ctx) {
        solver.factor(ctx, 1e100, Σ);
#if WITH_UPDATES
        solver.update(ctx, ΔΣ);
#endif
        solver.solve_forward(ctx, ux, λ);
        solver.solve_reverse(ctx, ux, λ);
        solver.residual_dynamics_constr(ctx, ux, λ_initial, Mxb);
        solver.transposed_dynamics_constr(ctx, λ, Mᵀλ);
        solver.cost_gradient(ctx, ux, -1, ux_initial, 0, grad);
        solver.general_constr(ctx, ux, DCux);
        ctx.arrive_and_wait();
        if (ctx.is_master()) // TODO
            Solver::compact_blas::xhadamard(simdify(Σ2), simdify(DCux));
        ctx.arrive_and_wait();
        solver.transposed_general_constr(ctx, DCux, DCᵀΣDCux);
    });

    using std::pow;
    const auto ε = pow(std::numeric_limits<real_t>::epsilon(), 0.6);
    Solver::compact_blas::xadd_copy(simdify(grad), simdify(grad), simdify(DCᵀΣDCux), simdify(Mᵀλ));
    std::cout << "dynamics constraints: "
              << guanaqo::float_to_str(Solver::compact_blas::xnrminf(simdify(Mxb)))
              << "\nstationarity:         "
              << guanaqo::float_to_str(Solver::compact_blas::xnrminf(simdify(grad))) << "\n";
    EXPECT_LE(Solver::compact_blas::xnrminf(simdify(Mxb)), ε);
    EXPECT_LE(Solver::compact_blas::xnrminf(simdify(grad)), ε);

#if GUANAQO_WITH_TRACING
    {
        solver.parallel_ctx->run([](auto &ctx) { GUANAQO_TRACE("thread_id", ctx.index); });
        std::string name = std::format("factor_cyclic_new.csv");
        std::filesystem::path out_dir{"traces"};
        out_dir /= *cyqlone_commit_hash ? cyqlone_commit_hash : "unknown";
        out_dir /= solver.get_params_string();
        std::filesystem::create_directories(out_dir);
        std::ofstream csv{out_dir / name};
        guanaqo::TraceLogger::write_column_headings(csv) << '\n';
        for (const auto &log : guanaqo::get_trace_logger().get_logs())
            csv << log << '\n';
        std::cout << (out_dir / name) << std::endl;
    }
#endif

    if (std::ofstream f("sparse.csv"); f) {
        auto sp = solver.build_sparse(cocp, Σ_lin2);
        for (auto [r, c, x] : sp.iter_coo())
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("sparse_factor.csv"); f) {
        auto sp = solver.build_sparse_factor();
        for (auto [r, c, x] : sp.iter_coo())
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("sparse_diag.csv"); f) {
        auto sp = solver.build_sparse_diag();
        for (auto [r, c, x] : sp.iter_coo())
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("sol.csv"); f) {
        auto b = solver.build_rhs(ux, λ);
        for (auto x : b)
            f << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("res.csv"); f) {
        auto b = solver.build_rhs(Mᵀλ, Mxb);
        for (auto x : b)
            f << guanaqo::float_to_str(x) << '\n';
    }

    solver.parallel_ctx->run([&](auto &ctx) { solver.factor(ctx, 1e100, Σ2); });
    if (std::ofstream f("sparse_refactor.csv"); f) {
        auto sp = solver.build_sparse_factor();
        for (auto [r, c, x] : sp.iter_coo())
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
}

INSTANTIATE_TEST_SUITE_P(CyqloneSolverConfigs, CyqloneFactorTest,
                         ::testing::Values(cyqlone::SolveMethod::StairPCG,
                                           cyqlone::SolveMethod::JacobiPCG,
                                           cyqlone::SolveMethod::PCR),
                         ([](const ::testing::TestParamInfo<CyqloneFactorTest::ParamType> &info) {
                             return enum_name(info.param);
                         }));
