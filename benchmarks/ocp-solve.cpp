#include <cyqlone/cyqlone.hpp>
#include <batmat/assume.hpp>
#include <batmat/loop.hpp>
#include <batmat/openmp.h>
#include <benchmark/benchmark.h>
#include <guanaqo/eigen/span.hpp>
#include <guanaqo/openmp.h>
#include <batmat-version.h>
#include <cyqlone-version.h>
#include <guanaqo-version.h>
#include <hyhound-version.h>
#include <omp.h>

#include <guanaqo/trace.hpp>
#include <hyhound/ocp/riccati.hpp>
#include <hyhound/ocp/schur.hpp>

#include <experimental/simd>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <utility>

#if WITH_BLASFEO
#include "riccati-blasfeo.hpp"
#include <blasfeo.h>
#endif

using namespace hyhound;
using namespace hyhound::ocp;
namespace stdx = std::experimental;
using guanaqo::as_span;

#if GUANAQO_WITH_TRACING
std::map<std::tuple<std::string, std::string>, std::filesystem::path> traces;
template <class T = void>
void trace(auto &&fun, const auto &name, const auto &params, T *solver = nullptr) {
    std::string filename = std::format("{}.csv", name);
    std::filesystem::path out_dir{"traces"};
    out_dir /= *cyqlone_commit_hash ? cyqlone_commit_hash : "unknown";
    out_dir /= params;
    std::filesystem::path out_file = out_dir / filename;
    if (auto [_, ins] = traces.insert({{name, params}, out_file}); !ins)
        return;
    fun();
    guanaqo::trace_logger.reset();
    fun();
    if constexpr (!std::is_void_v<T>)
        if (solver)
            solver->parallel_ctx->run([](auto &ctx) { GUANAQO_TRACE("thread_id", ctx.index); });
    std::filesystem::create_directories(out_dir);
    std::ofstream csv{out_file};
    guanaqo::TraceLogger::write_column_headings(csv) << '\n';
    for (const auto &log : guanaqo::trace_logger.get_logs())
        csv << log << '\n';
}
void print_traces(std::ostream &os) {
    for (const auto &[_, pth] : traces)
        os << pth << '\n';
}
#else
void trace(auto &&, const auto &, const auto &) {}
void print_traces(std::ostream &) {}
#endif

auto generate_ocp(benchmark::State &state) {
    using std::exp2;
    std::mt19937 rng{321};
    std::normal_distribution<real_t> nrml{0, 10};

    OCPDataRiccati ocp{.N  = static_cast<index_t>(state.range(0)),
                       .nx = static_cast<index_t>(state.range(1)),
                       .nu = static_cast<index_t>(state.range(2)),
                       .ny = static_cast<index_t>(state.range(3))};
    ocp.init_random(123);

    mat Σ = mat::Zero(ocp.ny, ocp.N + 1);
    std::ranges::generate(Σ.reshaped(), [&] { return exp2(nrml(rng)); });
    return std::pair{std::move(ocp), std::move(Σ)};
}

template <int VL>
auto build_cyqlone_solver(const OCPDataRiccati &ocp_ric, index_t lP) {
    using namespace ::batmat::linalg;
    using namespace ::cyqlone;
    LinearOCPStorage ocp{.dim{.N_horiz = ocp_ric.N,
                              .nx      = ocp_ric.nx,
                              .nu      = ocp_ric.nu,
                              .ny      = ocp_ric.ny,
                              .ny_N    = ocp_ric.ny}};
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    for (index_t i = 0; i < N; ++i) {
        as_eigen(ocp.A(i))       = ocp_ric.A(i);
        as_eigen(ocp.B(i))       = ocp_ric.B(i);
        as_eigen(ocp.C(i))       = ocp_ric.C(i);
        as_eigen(ocp.D(i))       = ocp_ric.D(i);
        as_eigen(ocp.Q(i))       = ocp_ric.Q(i);
        as_eigen(ocp.S(i))       = ocp_ric.S(i).transpose();
        as_eigen(ocp.S_trans(i)) = ocp_ric.S(i);
        as_eigen(ocp.R(i))       = ocp_ric.R(i);
    }
    as_eigen(ocp.C(N)) = ocp_ric.C(N);
    as_eigen(ocp.Q(N)) = ocp_ric.Q(N);
    using Solver       = CyqloneSolver<VL, real_t, StorageOrder::ColMajor>;
    auto cocp          = CyqloneStorage<real_t>::build(ocp);
    return Solver::build(cocp, lP + Solver::lvl);
}

void bm_factor_riccati(benchmark::State &state) {
    auto [ocp, Σ] = generate_ocp(state);
    RiccatiFactor fac{.ocp = ocp};
    for (auto _ : state)
        factor(fac, Σ);
    auto params = std::format("nx={}-nu={}-ny={}-N={}-thr={}", ocp.nx, ocp.nu, ocp.ny, ocp.N, 1);
    trace([&] { factor(fac, Σ); }, "factor_riccati", params);
}

void bm_solve_riccati(benchmark::State &state) {
    auto [ocp, Σ] = generate_ocp(state);
    RiccatiFactor fac{.ocp = ocp};
    factor(fac, Σ);
    for (auto _ : state)
        solve(fac);
    auto params = std::format("nx={}-nu={}-ny={}-N={}-thr={}", ocp.nx, ocp.nu, ocp.ny, ocp.N, 1);
    trace([&] { solve(fac); }, "solve_riccati", params);
}

void bm_update_riccati(benchmark::State &state) {
    using std::exp2;
    std::mt19937 rng{54321};
    std::normal_distribution<real_t> nrml{0, 10};
    std::bernoulli_distribution bern{0.25};

    auto [ocp, Σ] = generate_ocp(state);
    RiccatiFactor fac{.ocp = ocp};
    mat ΔΣ          = Σ;
    const auto prep = [&] {
        factor(fac, Σ);
        std::ranges::generate(ΔΣ.reshaped(),
                              [&] { return bern(rng) ? exp2(nrml(rng)) : real_t{0}; });
    };
    for (auto _ : state) {
        state.PauseTiming();
        prep();
        state.ResumeTiming();
        update(fac, ΔΣ);
    }
    auto params = std::format("nx={}-nu={}-ny={}-N={}-thr={}-upd={}", ocp.nx, ocp.nu, ocp.ny, ocp.N,
                              1, bern.p());
    prep();
    trace([&] { update(fac, ΔΣ); }, "update_riccati", params);
}

void bm_factor_schur(benchmark::State &state) {
    auto [ocp, Σ] = generate_ocp(state);
    auto ocp_sch  = OCPDataSchur::from_riccati(ocp);
    SchurFactor factor_sch{.ocp = ocp_sch};
    for (auto _ : state)
        factor(factor_sch, Σ);
    auto params = std::format("nx={}-nu={}-ny={}-N={}-thr={}", ocp.nx, ocp.nu, ocp.ny, ocp.N, 1);
    trace([&] { factor(factor_sch, Σ); }, "factor_schur", params);
}

void bm_solve_schur(benchmark::State &state) {
    auto [ocp, Σ] = generate_ocp(state);
    auto ocp_sch  = OCPDataSchur::from_riccati(ocp);
    SchurFactor factor_sch{.ocp = ocp_sch};
    factor(factor_sch, Σ);
    for (auto _ : state)
        solve(factor_sch);
    auto params = std::format("nx={}-nu={}-ny={}-N={}-thr={}", ocp.nx, ocp.nu, ocp.ny, ocp.N, 1);
    trace([&] { solve(factor_sch); }, "solve_schur", params);
}

void bm_update_schur(benchmark::State &state) {
    using std::exp2;
    std::mt19937 rng{54321};
    std::normal_distribution<real_t> nrml{0, 10};
    std::bernoulli_distribution bern{0.25};

    auto [ocp, Σ] = generate_ocp(state);
    auto ocp_sch  = OCPDataSchur::from_riccati(ocp);
    SchurFactor factor_sch{.ocp = ocp_sch};
    mat ΔΣ = Σ;

    for (auto _ : state) {
        state.PauseTiming();
        factor(factor_sch, Σ);
        std::ranges::generate(ΔΣ.reshaped(),
                              [&] { return bern(rng) ? exp2(nrml(rng)) : real_t{0}; });
        state.ResumeTiming();
        update(factor_sch, ΔΣ);
    }
}

template <int VL>
void bm_factor_cyqlone(benchmark::State &state) {
    using batmat::linalg::StorageOrder;
    auto [ocp, Σ]                           = generate_ocp(state);
    const auto lP                           = static_cast<index_t>(state.range(4));
    auto solver                             = build_cyqlone_solver<VL>(ocp, lP);
    solver.parallel_ctx->barrier.spin_count = std::numeric_limits<uint32_t>::max();
    GUANAQO_IF_ITT(solver.parallel_ctx->run(
        [](auto &ctx) { __itt_thread_set_name(std::format("OMP({})", ctx.index).c_str()); }));
    auto Σ_packed = solver.initialize_general_constraints();
    solver.pack_constraints(as_span(Σ.reshaped()), Σ_packed);
    const auto do_factor = [&] {
        solver.parallel_ctx->run([&](auto &ctx) { solver.factor(ctx, 1e100, Σ_packed); });
    };
    for (auto _ : state)
        do_factor();
    const std::string_view pcg = solver.use_stair_preconditioner ? "stair" : "jacobi";
    const auto params =
        std::format("nx={}-nu={}-ny={}-N={}-thr={}-vl={}-pcg={}{}-{}", solver.nx, solver.nu,
                    solver.ny, solver.N_horiz, 1 << lP, VL, pcg, solver.alt ? "-alt" : "",
                    solver.default_order == StorageOrder::RowMajor ? "rm" : "cm");
    trace([&] { do_factor(); }, "factor_cyqlone", params, &solver);
}

#if WITH_BLASFEO

void bm_factor_riccati_blasfeo(benchmark::State &state) {
    auto [ocp, Σ]    = generate_ocp(state);
    auto ocp_blasfeo = cyqlone::blasfeo::OCPDataRiccati::from_riccati(ocp);
    cyqlone::blasfeo::RiccatiFactor fac{.ocp = ocp_blasfeo};
    for (auto _ : state)
        factor(fac, Σ);
    auto params = std::format("nx={}-nu={}-ny={}-N={}-thr={}", ocp.nx, ocp.nu, ocp.ny, ocp.N, 1);
    trace([&] { factor(fac, Σ); }, "factor_riccati_blasfeo", params);
}

#endif

std::vector<benchmark::internal::Benchmark *> benchmarks;
void register_benchmark(benchmark::internal::Benchmark *bm) { benchmarks.push_back(bm); }
#define OCP_BENCHMARK(...) BENCHMARK(__VA_ARGS__)->Apply(register_benchmark)

OCP_BENCHMARK(bm_factor_riccati);
// OCP_BENCHMARK(bm_solve_riccati);
// OCP_BENCHMARK(bm_update_riccati);
OCP_BENCHMARK(bm_factor_schur);
// OCP_BENCHMARK(bm_solve_schur);
// OCP_BENCHMARK(bm_update_schur);
#if WITH_BLASFEO
OCP_BENCHMARK(bm_factor_riccati_blasfeo);
#endif
OCP_BENCHMARK(bm_factor_cyqlone<4>);
OCP_BENCHMARK(bm_factor_cyqlone<8>);
// OCP_BENCHMARK(bm_solve_cyqlone);

enum class BenchmarkType { None, vary_N, vary_N_pow_2, vary_nu, vary_ny, vary_nx_frac };

int main(int argc, char **argv) {
    if (argc < 1)
        return 1;
    // Parse command-line arguments
    std::vector<char *> argvv{argv, argv + argc};
    int64_t N = 32, nx = 16, nu = 8, ny = 8, step = 1,
            lP = BATMAT_OMP_IF_ELSE(
                std::bit_width(static_cast<unsigned>(omp_get_num_threads())) - 1, 0);
    BenchmarkType type = BenchmarkType::None;
    for (auto it = argvv.begin(); it != argvv.end();) {
        std::string_view arg = *it;
        if (std::string_view flag = "--N="; arg.starts_with(flag))
            N = std::stoi(std::string(arg.substr(flag.size())));
        else if (std::string_view flag = "--nx="; arg.starts_with(flag))
            nx = std::stoi(std::string(arg.substr(flag.size())));
        else if (std::string_view flag = "--nu="; arg.starts_with(flag))
            nu = std::stoi(std::string(arg.substr(flag.size())));
        else if (std::string_view flag = "--ny="; arg.starts_with(flag))
            ny = std::stoi(std::string(arg.substr(flag.size())));
        else if (std::string_view flag = "--step="; arg.starts_with(flag))
            step = std::stoi(std::string(arg.substr(flag.size())));
        else if (std::string_view flag = "--lP="; arg.starts_with(flag))
            lP = std::stoi(std::string(arg.substr(flag.size())));
        else if (arg == "--vary-N")
            type = BenchmarkType::vary_N;
        else if (arg == "--vary-N-pow-2")
            type = BenchmarkType::vary_N_pow_2;
        else if (arg == "--vary-nu")
            type = BenchmarkType::vary_nu;
        else if (arg == "--vary-ny")
            type = BenchmarkType::vary_ny;
        else if (arg == "--vary-nx-frac")
            type = BenchmarkType::vary_nx_frac;
        else {
            ++it;
            continue;
        }
        it = argvv.erase(it);
    }

    for (auto *bm : benchmarks) {
        bm->MeasureProcessCPUTime()->UseRealTime();
        bm->ArgNames({"N", "nx", "nu", "ny", "lP"});
        switch (type) {
            case BenchmarkType::None: bm->Args({N, nx, nu, ny, lP}); break;
            case BenchmarkType::vary_N:
                for (int64_t i = step; i <= N; i += step)
                    bm->Args({i, nx, nu, ny, lP});
                break;
            case BenchmarkType::vary_N_pow_2:
                for (int64_t i = 8; i <= N; i <<= step)
                    for (int64_t lPi = 0; 1 << (lPi + 2) <= i; lPi += 1)
                        bm->Args({i, nx, nu, ny, lPi});
                break;
            case BenchmarkType::vary_nu:
                for (int64_t i = step; i <= nu; i += step)
                    bm->Args({N, nx, i, ny, lP});
                break;
            case BenchmarkType::vary_ny:
                for (int64_t i = step; i <= ny; i += step)
                    bm->Args({N, nx, nu, i, lP});
                break;
            case BenchmarkType::vary_nx_frac:
                for (int64_t i = step; i <= nx; i += step)
                    bm->Args({N, i, (i * nu + (nx - 1) / 2) / nx, ny, lP});
                break;
            default: BATMAT_ASSUME(false);
        }
    }

    argc = static_cast<int>(argvv.size());
    ::benchmark::Initialize(&argc, argvv.data());
    if (::benchmark::ReportUnrecognizedArguments(argc, argvv.data()))
        return 1;
#if BATMAT_WITH_OPENMP
    benchmark::AddCustomContext("OMP_NUM_THREADS", std::to_string(omp_get_max_threads()));
#endif
    benchmark::AddCustomContext("cyqlone_build_time", cyqlone_build_time);
    benchmark::AddCustomContext("cyqlone_commit_hash", cyqlone_commit_hash);
    benchmark::AddCustomContext("batmat_build_time", batmat_build_time);
    benchmark::AddCustomContext("batmat_commit_hash", batmat_commit_hash);
    benchmark::AddCustomContext("hyhound_build_time", hyhound_build_time);
    benchmark::AddCustomContext("hyhound_commit_hash", hyhound_commit_hash);
    benchmark::AddCustomContext("guanaqo_build_time", guanaqo_build_time);
    benchmark::AddCustomContext("guanaqo_commit_hash", guanaqo_commit_hash);
#if defined(__AVX512F__)
    benchmark::AddCustomContext("arch", "avx512f");
#elif defined(__AVX2__)
    benchmark::AddCustomContext("arch", "avx2");
#elif defined(__AVX__)
    benchmark::AddCustomContext("arch", "avx");
#elif defined(__SSE3__)
    benchmark::AddCustomContext("arch", "sse3");
#endif
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    print_traces(std::cout);
}
