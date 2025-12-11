#include <benchmark/benchmark.h>

#include <cyqlone/matio.hpp>
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/example-problems/spring-mass.hpp>
#include <cyqlone/qpalm/settings.hpp>
#include <cyqlone/qpalm/solver.hpp>
#include <cyqlone/qpalm/status.hpp>
#include <CLI/CLI.hpp>
#include <batmat/openmp.h>
#include <batmat-version.h>
#include <cyqlone-version.h>
#include <algorithm>
#include <format>
#include <generator>
#include <random>

#include "hpipm.hpp"

auto counter(auto x) {
    return benchmark::Counter{static_cast<double>(x), benchmark::Counter::kDefaults};
}
auto counter_avg(auto x) {
    return benchmark::Counter{static_cast<double>(x), benchmark::Counter::kAvgIterations};
}

enum class ProblemType {
    WangBoyd2008,
    WangBoyd2008Width,
    Domahidi2012,
    ActiveStateConstr,
};
const std::map<std::string, ProblemType> problem_type_map{
    {"wang-boyd-2008", ProblemType::WangBoyd2008},
    {"wang-boyd-2008-width", ProblemType::WangBoyd2008Width},
    {"domahidi-2012", ProblemType::Domahidi2012},
    {"active-state-constr", ProblemType::ActiveStateConstr},
};

struct Options {
    bool warm           = false;
    bool no_updates     = false;
    int log_parallelism = 5;
    bool rm             = false;
    bool cm             = true;
    bool pcr            = true;
    bool hpipm          = true;
    bool v8             = false;
    bool warm_copy      = false;
    std::vector<int> horizon{32, 64, 96, 128, 192, 256};
    std::vector<int> masses{6, 12, 30};
    uint64_t num_instances   = 50;
    uint64_t seed            = 0;
    ProblemType problem_type = ProblemType::WangBoyd2008;
    double pcr_max_update_fraction = 0.25;
    double cr_max_update_fraction  = 0.9;
    double changing_constr_factor  = 0.01;
};

namespace qp = cyqlone::qpalm;
using cyqlone::index_t;
using cyqlone::real_t;
using qp::problems::SpringMassParams;
using seconds = std::chrono::duration<double>;

qp::problems::SpringMassProblem create_problem(const SpringMassParams &params) {
    auto problem = qp::problems::spring_mass(params);
    // Add a random disturbance to the initial position of the masses
    std::seed_seq seed{params.seed, params.seed >> 32};
    std::mt19937 rng{seed};
    using dist_t = std::uniform_real_distribution<real_t>;
    auto dist    = params.p_max > 1 ? dist_t{-3, 3} : dist_t{-0.6, -0.1};
    for (index_t r = 0; r < static_cast<index_t>(params.masses.size()); ++r)
        problem.ocp.b(0)(r, 0) += dist(rng);
    return problem;
}

template <index_t VL, qp::StorageOrder Order = qp::StorageOrder::RowMajor>
void stress(const SpringMassParams &params, qp::CyqloneBackendSettings backend_settings,
            qp::Settings settings, bool warm = false) {
    auto problem = create_problem(params);
    // Build a QPALM Cyqlone solver
    auto ocp     = cyqlone::CyqloneStorage<>::build(problem.ocp);
    auto backend = qp::make_qpalm_cyqlone_backend<VL, Order>(ocp, {}, backend_settings);
    qp::Solver<qp::CyqloneBackend<VL, Order> *> qpalm{backend.get(), settings};
    qpalm();
    const auto inner_iter = qpalm.stats->inner_iter;
    for (index_t i = 0; i < 1000; ++i) {
        qpalm();
        if (qpalm.stats->inner_iter != inner_iter)
            throw std::runtime_error("Inconsistent inner iterations detected");
    }
}

template <index_t VL, qp::StorageOrder Order = qp::StorageOrder::RowMajor>
void run_benchmark(benchmark::State &state, const SpringMassParams &params,
                   qp::CyqloneBackendSettings backend_settings, qp::Settings settings,
                   bool warm = false) {
    auto problem = create_problem(params);
    if (problem.ocp.dim.N_horiz <= (1 << (backend_settings.log_processors - 1)))
        state.SkipWithMessage("Problem too small for the selected number of processors");
    // Build a QPALM Cyqlone solver
    auto ocp     = cyqlone::CyqloneStorage<>::build(problem.ocp);
    auto backend = qp::make_qpalm_cyqlone_backend<VL, Order>(ocp, {}, backend_settings);
    qp::Solver<qp::CyqloneBackend<VL, Order> *> qpalm{backend.get(), settings};
    // Main benchmark
    qp::SolverStatus status;
    double time_solve = 0, time_active_set_change = 0, time_line_search = 0,
           time_recompute_outer = 0, time_recompute_inner = 0, time_mat_vec = 0;
    qpalm(); // Warm up
    if (warm) {
        const auto nx = problem.ocp.dim.nx, nu = problem.ocp.dim.nu;
        qpalm.warm_start_solution();
        auto x = qpalm.get_solution();
        std::copy_n(x.data() + nu, nx, problem.ocp.b(0).data);
        ocp.update(problem.ocp);
        update_qpalm_cyqlone_backend(*qpalm.backend, ocp);
    }
    for (auto _ : state) {
        status = qpalm();
        state.SetIterationTime(seconds(qpalm.stats->timings.total.wall_time).count());
        time_solve += seconds(qpalm.stats->timings.solve.wall_time).count();
        time_active_set_change += seconds(qpalm.stats->timings.active_set_change.wall_time).count();
        time_line_search += seconds(qpalm.stats->timings.line_search.wall_time).count();
        time_recompute_outer += seconds(qpalm.stats->timings.recompute_outer.wall_time).count();
        time_recompute_inner += seconds(qpalm.stats->timings.recompute_inner.wall_time).count();
        time_mat_vec += seconds(qpalm.stats->timings.mat_vec_A.wall_time).count();
        time_mat_vec += seconds(qpalm.stats->timings.mat_vec_AT.wall_time).count();
        time_mat_vec += seconds(qpalm.stats->timings.mat_vec_M.wall_time).count();
        time_mat_vec += seconds(qpalm.stats->timings.mat_vec_MT.wall_time).count();
        time_mat_vec += seconds(qpalm.stats->timings.mat_vec_Q.wall_time).count();
    }
    auto sol       = ocp.reconstruct_solution(problem.ocp, qpalm.get_solution(),
                                              qpalm.get_inequality_multipliers(),
                                              qpalm.get_equality_multipliers());
    auto kkt_error = problem.ocp.compute_kkt_error(sol);

    state.counters["inner_iter"]             = counter(qpalm.stats->inner_iter);
    state.counters["outer_iter"]             = counter(qpalm.stats->outer_iter);
    state.counters["num_factor"]             = counter(qpalm.stats_backend->num_factor);
    state.counters["num_updates"]            = counter(qpalm.stats_backend->num_updates);
    state.counters["rank_updates"]           = counter(qpalm.stats_backend->rank_updates);
    state.counters["status"]                 = counter(status);
    state.counters["success"]                = counter(status == qp::SolverStatus::Converged);
    state.counters["time_solve"]             = counter_avg(time_solve);
    state.counters["time_active_set_change"] = counter_avg(time_active_set_change);
    state.counters["time_line_search"]       = counter_avg(time_line_search);
    state.counters["time_recompute_outer"]   = counter_avg(time_recompute_outer);
    state.counters["time_recompute_inner"]   = counter_avg(time_recompute_inner);
    state.counters["time_mat_vec"]           = counter_avg(time_mat_vec);
    state.counters["stationarity"]           = counter(kkt_error.stationarity);
    state.counters["equality_residual"]      = counter(kkt_error.equality_residual);
    state.counters["inequality_residual"]    = counter(kkt_error.inequality_residual);
    state.counters["complementarity"]        = counter(kkt_error.complementarity);
}

enum class WarmStartHPIPM {
    NoWarmStart,
    WarmZero,
    WarmCopy,
    WarmShift,
};

#if WITH_HPIPM
void run_benchmark_hpipm(benchmark::State &state, const SpringMassParams &params,
                         WarmStartHPIPM warm = WarmStartHPIPM::NoWarmStart) {
    auto problem     = create_problem(params);
    auto qp_data     = build_hpipm_qp(problem.ocp);
    auto solver_data = create_hpipm_solver(qp_data, warm != WarmStartHPIPM::NoWarmStart ? 1 : 0);
    // Main benchmark
    solve_hpipm(*qp_data, *solver_data); // Warm up
    std::optional<cyqlone::LinearOCPStorage::Solution> prev_sol;
    if (warm != WarmStartHPIPM::NoWarmStart) {
        prev_sol = get_solution_hpipm(*solver_data);
        update_x0_hpipm(problem.ocp, *qp_data, prev_sol->solution);
        if (warm == WarmStartHPIPM::WarmZero)
            std::ranges::fill(prev_sol->solution, real_t{0});
        else if (warm == WarmStartHPIPM::WarmShift)
            shift_solution_hpipm(*qp_data, prev_sol->solution);
    }
    for (auto _ : state) {
        if (prev_sol)
            warm_start_hpipm(*solver_data, prev_sol->solution);
        auto time_solve = solve_hpipm(*qp_data, *solver_data);
        state.SetIterationTime(seconds(time_solve).count());
    }
    auto sol                              = get_solution_hpipm(*solver_data);
    auto kkt_error                        = problem.ocp.compute_kkt_error(sol);
    auto stats                            = get_stats_hpipm(*solver_data);
    state.counters["iter"]                = counter(stats.iter);
    state.counters["status"]              = counter(stats.status);
    state.counters["success"]             = counter(stats.status == 0);
    state.counters["max_res_stat"]        = counter(stats.max_res_stat);
    state.counters["max_res_eq"]          = counter(stats.max_res_eq);
    state.counters["max_res_ineq"]        = counter(stats.max_res_ineq);
    state.counters["max_res_comp"]        = counter(stats.max_res_comp);
    state.counters["stationarity"]        = counter(kkt_error.stationarity);
    state.counters["equality_residual"]   = counter(kkt_error.equality_residual);
    state.counters["inequality_residual"] = counter(kkt_error.inequality_residual);
    state.counters["complementarity"]     = counter(kkt_error.complementarity);
}
#endif

struct Problem {
    std::string name;
    SpringMassParams params;
};

std::generator<Problem> get_spring_mass_params(const Options &opts) {
    using std::format;
    for (auto M : opts.masses)
        for (auto N : opts.horizon)
            for (uint64_t seed = opts.seed; seed < opts.seed + opts.num_instances; ++seed)
                switch (opts.problem_type) {
                    case ProblemType::WangBoyd2008:
                        co_yield {
                            .name   = format("spring-mass(M={},N={},seed={}) wb2008", M, N, seed),
                            .params = SpringMassParams::wang_boyd_2008(M, N, seed),
                        };
                        break;
                    case ProblemType::WangBoyd2008Width:
                        co_yield {
                            .name   = format("spring-mass(M={},N={},seed={}) wb2008w", M, N, seed),
                            .params = SpringMassParams::wang_boyd_2008_width(M, N, seed, 1),
                        };
                        break;
                    case ProblemType::Domahidi2012:
                        co_yield {
                            .name   = format("spring-mass(M={},N={},seed={}) d2012", M, N, seed),
                            .params = SpringMassParams::domahidi_2012(M, N, seed),
                        };
                        break;
                    case ProblemType::ActiveStateConstr:
                        co_yield {
                            .name   = format("spring-mass(M={},N={},seed={}) act", M, N, seed),
                            .params = SpringMassParams::active_state_constr(M, N, seed),
                        };
                        break;
                    default: throw std::runtime_error("Unknown problem type");
                }
}

struct Solver {
    std::string name;
    std::function<void(benchmark::State &, SpringMassParams)> run;
};

std::generator<Solver> get_solvers(const Options &opts) {
    using enum qp::StorageOrder;
    qp::CyqloneBackendSettings backend{
        .log_processors          = opts.log_parallelism,
        .changing_constr_factor  = opts.changing_constr_factor,
        .max_update_count        = 20,
        .pcr_max_update_fraction = opts.pcr_max_update_fraction,
        .cr_max_update_fraction  = opts.cr_max_update_fraction,
    };
    qp::Settings settings{
        .tolerance         = 1e-8,
        .dual_tolerance    = 1e-8,
        .initial_penalty_y = 20,
        .verbose           = false,
    };
    if (opts.pcr)
        backend.solve_method = cyqlone::SolveMethod::PCR;
    if (opts.warm) {
        backend.strategy = qp::WarmStartingStrategy::Zeros;
        co_yield {std::format("cyqlone(v=4,p={},zero)", 1 << (backend.log_processors - 2)),
                  [=](benchmark::State &state, const SpringMassParams &params) {
                      run_benchmark<4>(state, params, backend, settings, true);
                  }};
        settings.initial_penalty_y       = 1e4;
        settings.initial_inner_tolerance = 1e-4;
        if (opts.warm_copy) {
            backend.strategy = qp::WarmStartingStrategy::Copy;
            co_yield {std::format("cyqlone(v=4,p={},copy)", 1 << (backend.log_processors - 2)),
                      [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark<4>(state, params, backend, settings, true);
                      }};
        }
        backend.strategy = qp::WarmStartingStrategy::Shift;
        co_yield {std::format("cyqlone(v=4,p={},shift)", 1 << (backend.log_processors - 2)),
                  [=](benchmark::State &state, const SpringMassParams &params) {
                      run_benchmark<4>(state, params, backend, settings, true);
                  }};
#if WITH_HPIPM
        if (opts.hpipm) {
            co_yield {"hpipm(zero)", [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark_hpipm(state, params, WarmStartHPIPM::WarmZero);
                      }};
            if (opts.warm_copy)
                co_yield {"hpipm(copy)",
                          [=](benchmark::State &state, const SpringMassParams &params) {
                              run_benchmark_hpipm(state, params, WarmStartHPIPM::WarmCopy);
                          }};
            co_yield {"hpipm(shift)", [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark_hpipm(state, params, WarmStartHPIPM::WarmShift);
                      }};
        }
#endif
    } else {
        if (opts.rm)
            co_yield {std::format("cyqlone(v=4,p={},rm)", 1 << (backend.log_processors - 2)),
                      [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark<4, RowMajor>(state, params, backend, settings);
                      }};
        if (opts.cm)
            co_yield {std::format("cyqlone(v=4,p={},cm)", 1 << (backend.log_processors - 2)),
                      [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark<4, ColMajor>(state, params, backend, settings);
                      }};
        if (opts.v8) {
            ++backend.log_processors;
            co_yield {std::format("cyqlone(v=8,p={},cm)", 1 << (backend.log_processors - 3)),
                      [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark<8, ColMajor>(state, params, backend, settings);
                      }};
            --backend.log_processors;
        }
        if (opts.no_updates) {
            backend.max_update_count = 0;
            co_yield {std::format("cyqlone(v=4,p={},upd={})", 1 << (backend.log_processors - 2),
                                  backend.max_update_count),
                      [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark<4>(state, params, backend, settings);
                      }};
        }
#if WITH_HPIPM
        if (opts.hpipm) {
            co_yield {"hpipm", [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark_hpipm(state, params);
                      }};
        }
#endif
    }
}

void register_benchmarks(const Options &opts) {
    for (auto problem : get_spring_mass_params(opts))
        for (auto solver : get_solvers(opts))
            benchmark::RegisterBenchmark(std::format("{} - {}", problem.name, solver.name),
                                         [params = problem.params, run = std::move(solver.run)](
                                             benchmark::State &state) { run(state, params); })
                ->MeasureProcessCPUTime()
                ->UseManualTime()
                ->Unit(benchmark::kMillisecond)
                ->ComputeStatistics("min", [](auto &v) { return *std::ranges::min_element(v); })
                ->ComputeStatistics("max", [](auto &v) { return *std::ranges::max_element(v); });
}

int main(int argc, char **argv) try {
    char *const program = argv[0];
    Options opts;
    CLI::App app{"Spring-Mass Benchmarks"};
    app.allow_extras();
    app.add_flag("--warm,!--cold", opts.warm, "Benchmark with warm starting");
    app.add_option("--horizon,-N", opts.horizon, "Specify a horizon length to benchmark");
    app.add_option("--masses,-M", opts.masses, "Specify the number of masses to benchmark");
    app.add_option("--num-instances,-I", opts.num_instances,
                   "Number of random problem instances to generate for each (M,N) pair");
    app.add_option("--seed,-s", opts.seed, "Random seed for problem instance generation");
    app.add_flag("--no-updates", opts.no_updates, "Compare to the Cyqlone backend without updates");
    app.add_option("--log-parallelism,-P", opts.log_parallelism,
                   "Log2 of the amount of parallelism to use in the Cyqlone backend");
    app.add_flag("--rm,!--no-rm", opts.rm, "Use row-major (default) storage");
    app.add_flag("--cm,!--no-cm", opts.cm, "Use column-major storage");
    app.add_flag("--pcr,!--pcg", opts.pcr, "Use parallel cyclic reduction in the Cyqlone backend");
    app.add_flag("--hpipm,!--no-hpipm", opts.hpipm, "Use HPIPM solver for comparison");
    app.add_flag("--v8,!--no-v8", opts.v8, "Include vector length 8 for the Cyqlone backend");
    app.add_option("--problem,-p", opts.problem_type, "Problem type to benchmark")
        ->transform(CLI::CheckedTransformer(problem_type_map, CLI::ignore_case));
    app.add_option("--pcr-max-update-fraction", opts.pcr_max_update_fraction,
                   "Maximum update rank fraction when using PCR");
    app.add_option("--cr-max-update-fraction", opts.cr_max_update_fraction,
                   "Maximum update rank fraction when using CR");
    app.add_option("--changing-constr-factor", opts.changing_constr_factor,
                   "Changing constraints factor for the Cyqlone backend");
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    register_benchmarks(opts);
    std::vector bm_args = app.remaining(true);
    if (!bm_args.empty() && bm_args.front() == "--")
        bm_args.erase(bm_args.begin());
    std::vector<char *> bm_argv(bm_args.size() + 2);
    std::ranges::transform(bm_args, bm_argv.begin() + 1, [](auto &s) { return s.data(); });
    bm_argv[0]   = program;
    auto bm_argc = static_cast<int>(bm_args.size()) + 1;
    benchmark::Initialize(&bm_argc, bm_argv.data());
    if (benchmark::ReportUnrecognizedArguments(bm_argc, bm_argv.data()))
        return 1;
#if BATMAT_WITH_OPENMP
    benchmark::AddCustomContext("OMP_NUM_THREADS", std::to_string(omp_get_max_threads()));
#endif
    benchmark::AddCustomContext("batmat_build_time", batmat_build_time);
    benchmark::AddCustomContext("batmat_commit_hash", batmat_commit_hash);
    benchmark::AddCustomContext("cyqlone_build_time", cyqlone_build_time);
    benchmark::AddCustomContext("cyqlone_commit_hash", cyqlone_commit_hash);
#if defined(__AVX512F__)
    benchmark::AddCustomContext("arch", "avx512f");
#elif defined(__AVX2__)
    benchmark::AddCustomContext("arch", "avx2");
#elif defined(__AVX__)
    benchmark::AddCustomContext("arch", "avx");
#elif defined(__SSE3__)
    benchmark::AddCustomContext("arch", "sse3");
#endif
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
} catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
