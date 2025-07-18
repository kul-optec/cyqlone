#include <gtest/gtest.h>

#include <batmat/linalg/simdify.hpp>
#include <batmat/loop.hpp>
#include <batmat/openmp.h>
#include <batmat/thread-pool.hpp>
#include <cyqlone/cyqlone.hpp>
#include <cyqlone/random-ocp.hpp>
#include <guanaqo/print.hpp>
#include <guanaqo/trace.hpp>
#include <batmat-version.h>

#if GUANAQO_WITH_TRACING
#include <filesystem>
#endif
#include <fstream>
#include <iostream>
#include <limits>

using batmat::index_t;
using batmat::real_t;

TEST(Cyqlone, factor) {
    using namespace cyqlone;
    using ::batmat::linalg::simdify;

    const int log_n_threads = 2;

    BATMAT_OMP_IF(omp_set_num_threads(1 << log_n_threads));
    batmat::pool_set_num_threads(1 << log_n_threads);
    GUANAQO_IF_ITT(batmat::foreach_thread(
        [](index_t i, index_t) { __itt_thread_set_name(std::format("OMP({})", i).c_str()); }));

    using Solver     = CyqloneSolver<4, real_t, StorageOrder::RowMajor>;
    const index_t lP = log_n_threads + Solver::lvl;
    const index_t ny = 50, ny_0 = 25, ny_N = 25;
    OCPDim dim{.N_horiz = 91, .nx = 40, .nu = 30, .ny = ny, .ny_N = ny_N};
    const index_t nux = dim.nu + dim.nx, N = dim.N_horiz;
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny_0).set_constant(0);
    std::mt19937 rng(102030405);
    std::uniform_real_distribution<real_t> uni(-1, 1);
    std::bernoulli_distribution bern(0.01);
    std::vector<real_t> qr_lin(nux * N + dim.nx), b_eq_lin(nux * (N + 1)), b_lb_lin(ny * N + ny_N),
        b_ub_lin(ny * N + ny_N);
    std::ranges::generate(qr_lin, [&] { return uni(rng); });
    std::ranges::generate(b_eq_lin, [&] { return uni(rng); });
    std::ranges::generate(b_lb_lin, [&] { return uni(rng); });
    std::ranges::generate(b_ub_lin, [&] { return uni(rng); });
    auto cocp     = CyclicOCPStorage<real_t>::build(ocp, qr_lin, b_eq_lin, b_lb_lin, b_ub_lin);
    Solver solver = Solver::build(cocp, lP);

    const index_t nyM = std::max(ny, ny_0 + ny_N);
    std::vector<real_t> Σ_lin((N - 1) * ny + ny_0 + ny_N);
    Solver::matrix λ{{.depth = solver.ceil_N, .rows = dim.nx, .cols = 1}},
        ux{{.depth = solver.ceil_N, .rows = nux, .cols = 1}},
        Mᵀλ{{.depth = solver.ceil_N, .rows = nux, .cols = 1}},
        DCux{{.depth = solver.ceil_N, .rows = nyM, .cols = 1}},
        DCᵀΣDCux{{.depth = solver.ceil_N, .rows = nux, .cols = 1}},
        grad{{.depth = solver.ceil_N, .rows = nux, .cols = 1}},
        Mxb{{.depth = solver.ceil_N, .rows = dim.nx, .cols = 1}},
        Σ{{.depth = solver.ceil_N, .rows = dim.ny, .cols = 1}},
        Σ2{{.depth = solver.ceil_N, .rows = dim.ny, .cols = 1}},
        ΔΣ{{.depth = solver.ceil_N, .rows = dim.ny, .cols = 1}};
    std::ranges::generate(λ, [&] { return uni(rng); });
    std::ranges::generate(ux, [&] { return uni(rng); });
    std::ranges::generate(Σ_lin, [&] { return std::exp2(uni(rng)); });
    std::vector<real_t> Σ_lin2 = Σ_lin;
    for (auto &Σ2i : Σ_lin2)
        if (bern(rng))
            Σ2i = std::exp2(uni(rng));
    solver.pack_constraints(Σ_lin, Σ);
    solver.pack_constraints(Σ_lin2, Σ2);
    std::ranges::transform(Σ2, Σ, std::ranges::begin(ΔΣ), std::minus<>{});

    if (std::ofstream f("rhs.csv"); f) {
        auto b = solver.build_rhs(ux, λ);
        for (auto x : b)
            f << guanaqo::float_to_str(x) << '\n';
    }

    const bool alt        = true;
    const auto ux_initial = ux, λ_initial = λ;
    for (int i = 0; i < 500; ++i) {
        solver.factor(1e100, Σ, alt);
        solver.update(ΔΣ);
        solver.solve(ux, λ);
        solver.residual_dynamics_constr(ux, λ_initial, Mxb);
        solver.transposed_dynamics_constr(λ, Mᵀλ);
        solver.cost_gradient(ux, -1, ux_initial, 0, grad);
        solver.general_constr(ux, DCux);
        Solver::compact_blas::xhadamard(simdify(Σ2), simdify(DCux));
        solver.transposed_general_constr(DCux, DCᵀΣDCux);
        ux.view() = ux_initial.view();
        λ.view()  = λ_initial.view();
    }
#if GUANAQO_WITH_TRACING
    guanaqo::trace_logger.reset();
#endif
    solver.factor(1e100, Σ, alt);
    solver.update(ΔΣ);
    solver.solve(ux, λ);
    solver.residual_dynamics_constr(ux, λ_initial, Mxb);
    solver.transposed_dynamics_constr(λ, Mᵀλ);
    solver.cost_gradient(ux, -1, ux_initial, 0, grad);
    solver.general_constr(ux, DCux);
    Solver::compact_blas::xhadamard(simdify(Σ2), simdify(DCux));
    solver.transposed_general_constr(DCux, DCᵀΣDCux);

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
        batmat::foreach_thread([](index_t i, index_t) { GUANAQO_TRACE("thread_id", i); });
        const auto N     = solver.N_horiz;
        const auto VL    = solver.vl;
        std::string name = std::format("factor_cyclic_new.csv");
        std::filesystem::path out_dir{"traces"};
#if USE_JACOBI_PREC
        const std::string_view pcg = "jacobi";
#else
        const std::string_view pcg = "stair";
#endif
        out_dir /= *batmat_commit_hash ? batmat_commit_hash : "unknown";
        out_dir /=
            std::format("nx={}-nu={}-ny={}-N={}-thr={}-vl={}-pcg={}{}-{}", solver.nx, solver.nu,
                        solver.ny, N, 1 << log_n_threads, VL, pcg, alt ? "-alt" : "",
                        solver.default_order == StorageOrder::RowMajor ? "rm" : "cm");
        std::filesystem::create_directories(out_dir);
        std::ofstream csv{out_dir / name};
        guanaqo::TraceLogger::write_column_headings(csv) << '\n';
        for (const auto &log : guanaqo::trace_logger.get_logs())
            csv << log << '\n';
        std::cout << (out_dir / name) << std::endl;
    }
#endif

    if (std::ofstream f("sparse.csv"); f) {
        auto sp = solver.build_sparse(cocp, Σ_lin2);
        for (auto [r, c, x] : sp)
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("sparse_factor.csv"); f) {
        auto sp = solver.build_sparse_factor();
        for (auto [r, c, x] : sp)
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("sparse_diag.csv"); f) {
        auto sp = solver.build_sparse_diag();
        for (auto [r, c, x] : sp)
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
    if (std::ofstream f("sol.csv"); f) {
        auto b = solver.build_rhs(ux, λ);
        for (auto x : b)
            f << guanaqo::float_to_str(x) << '\n';
    }

    solver.factor(1e100, Σ2, alt);
    if (std::ofstream f("sparse_refactor.csv"); f) {
        auto sp = solver.build_sparse_factor();
        for (auto [r, c, x] : sp)
            f << r << ',' << c << ',' << guanaqo::float_to_str(x) << '\n';
    }
}
