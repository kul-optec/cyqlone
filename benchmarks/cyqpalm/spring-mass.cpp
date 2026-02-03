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
#include <filesystem>
#include <format>
#include <generator>
#include <random>
#include <stdexcept>
#include <utility>
namespace fs = std::filesystem;

#include "hpipm.hpp"

#if BATMAT_HAS_DOUBLE_VL_2 && defined(__ARM_NEON)
constexpr cyqlone::index_t v = 2;
#elif BATMAT_HAS_DOUBLE_VL_4
constexpr cyqlone::index_t v = 4;
#elif BATMAT_HAS_DOUBLE_VL_2
constexpr cyqlone::index_t v = 2;
#else
constexpr cyqlone::index_t v = 1;
#endif

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
    bool cold         = true;
    bool warm_shift   = true;
    bool warm_copy    = false;
    bool no_updates   = false;
    int parallelism   = 8;
    int vector_length = v;
    bool rm           = false;
    bool cm           = true;
    bool pcr          = true;
#if WITH_HPIPM
    bool hpipm = true;
#else
    bool hpipm = false;
#endif
    std::vector<int> horizon{32, 64, 96, 128, 192, 256};
    std::vector<int> masses{6, 12, 30};
    uint64_t num_instances            = 50;
    uint64_t seed                     = 0;
    ProblemType problem_type          = ProblemType::WangBoyd2008;
    double pcr_max_update_fraction    = 0.25;
    double cr_max_update_fraction     = 0.9;
    int parallel_solve_cr_threshold   = 10;
    int parallel_factor_pcr_threshold = 10;
    double changing_constr_factor     = 0.01;
    bool custom_reporter              = true;
    bool print_extra                  = false;
    bool use_color                    = false;
    std::string export_problem{};
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

template <index_t VL, qp::StorageOrder Order>
void run_benchmark(benchmark::State &state, const SpringMassParams &params,
                   qp::CyqloneBackendSettings backend_settings, qp::Settings settings,
                   bool warm = false) {
    if (backend_settings.processors < 2)
        return state.SkipWithMessage("Fewer than 2 processors are currently not supported.");
    auto problem = create_problem(params);
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

    state.counters["status"]              = counter(status);
    state.counters["success"]             = counter(status == qp::SolverStatus::Converged);
    state.counters["iter"]                = counter(qpalm.stats->inner_iter);
    state.counters["outer_iter"]          = counter(qpalm.stats->outer_iter);
    state.counters["num_factor"]          = counter(qpalm.stats_backend->num_factor);
    state.counters["num_upd"]             = counter(qpalm.stats_backend->num_updates);
    state.counters["rank_upd"]            = counter(qpalm.stats_backend->rank_updates);
    state.counters["t_solve"]             = counter_avg(time_solve);
    state.counters["t_active_set_change"] = counter_avg(time_active_set_change);
    state.counters["t_line_search"]       = counter_avg(time_line_search);
    state.counters["t_recompute_outer"]   = counter_avg(time_recompute_outer);
    state.counters["t_recompute_inner"]   = counter_avg(time_recompute_inner);
    state.counters["t_mat_vec"]           = counter_avg(time_mat_vec);
    state.counters["res_dual"]            = counter(kkt_error.stationarity);
    state.counters["res_eq"]              = counter(kkt_error.equality_residual);
    state.counters["res_ineq"]            = counter(kkt_error.inequality_residual);
    state.counters["compl"]               = counter(kkt_error.complementarity);
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
    auto sol                       = get_solution_hpipm(*solver_data);
    auto kkt_error                 = problem.ocp.compute_kkt_error(sol);
    auto stats                     = get_stats_hpipm(*solver_data);
    state.counters["status"]       = counter(stats.status);
    state.counters["success"]      = counter(stats.status == 0);
    state.counters["iter"]         = counter(stats.iter);
    state.counters["max_res_stat"] = counter(stats.max_res_stat);
    state.counters["max_res_eq"]   = counter(stats.max_res_eq);
    state.counters["max_res_ineq"] = counter(stats.max_res_ineq);
    state.counters["max_res_comp"] = counter(stats.max_res_comp);
    state.counters["res_dual"]     = counter(kkt_error.stationarity);
    state.counters["res_eq"]       = counter(kkt_error.equality_residual);
    state.counters["res_ineq"]     = counter(kkt_error.inequality_residual);
    state.counters["compl"]        = counter(kkt_error.complementarity);
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

void export_problem(const Options &opts) {
    if (opts.export_problem.empty())
        return;
    for (const auto &params : get_spring_mass_params(opts)) {
        auto problem      = create_problem(params.params);
        fs::path filename = opts.export_problem;
        if (fs::is_directory(filename))
            filename /= params.name + ".mat";
        auto mat = cyqlone::create_mat(filename);
        cyqlone::add_to_mat(mat.get(), problem.ocp);
        std::cout << "Exported problem " << params.name << " to " << filename << std::endl;
        break;
    }
}

struct Solver {
    std::string name;
    std::function<void(benchmark::State &, SpringMassParams)> run;
};

std::string_view order(qp::StorageOrder o) { return o == qp::StorageOrder::RowMajor ? "rm" : "cm"; }

template <index_t VL, qp::StorageOrder O>
std::generator<Solver> get_cyqlone_solvers(const Options &opts) {
    static constexpr auto cyqlone_solver = [](std::string_view name,
                                              const qp::CyqloneBackendSettings &backend,
                                              const qp::Settings &settings) {
        return Solver{
            std::format("cyqlone(p={},v={},{},{})", backend.processors, VL, order(O), name),
            [=](benchmark::State &state, const SpringMassParams &params) {
                run_benchmark<VL, O>(state, params, backend, settings, true);
            }};
    };
    qp::CyqloneBackendSettings backend{
        .processors             = opts.parallelism,
        .changing_constr_factor = opts.changing_constr_factor,
        .max_update_count       = 20,
        .tricyqle_params =
            {
                .pcr_max_update_fraction       = opts.pcr_max_update_fraction,
                .cr_max_update_fraction_Y0     = opts.cr_max_update_fraction,
                .parallel_solve_cr_threshold   = opts.parallel_solve_cr_threshold,
                .parallel_factor_pcr_threshold = opts.parallel_factor_pcr_threshold,
            },
    };
    qp::Settings settings{
        .tolerance         = 1e-8,
        .dual_tolerance    = 1e-8,
        .initial_penalty_y = 20,
        .verbose           = false,
    };
    qp::CyqloneBackendSettings backend_no_upd = backend;
    backend_no_upd.max_update_count           = 0;
    qp::Settings settings_warm                = settings;
    settings_warm.initial_penalty_y           = 1e4;
    settings_warm.initial_inner_tolerance     = 1e-4;
    if (opts.pcr)
        backend.tricyqle_params.solve_method = cyqlone::SolveMethod::PCR;
    if (opts.cold) {
        backend.strategy = backend_no_upd.strategy = qp::WarmStartingStrategy::Zeros;
        co_yield cyqlone_solver("zero", backend, settings);
        if (opts.no_updates)
            co_yield cyqlone_solver("zero,upd=0", backend_no_upd, settings);
    }
    if (opts.warm_shift) {
        backend.strategy = backend_no_upd.strategy = qp::WarmStartingStrategy::Shift;
        co_yield cyqlone_solver("shift", backend, settings_warm);
        if (opts.no_updates)
            co_yield cyqlone_solver("shift,upd=0", backend_no_upd, settings_warm);
    }
    if (opts.warm_copy) {
        backend.strategy = backend_no_upd.strategy = qp::WarmStartingStrategy::Copy;
        co_yield cyqlone_solver("copy", backend, settings_warm);
        if (opts.no_updates)
            co_yield cyqlone_solver("copy,upd=0", backend_no_upd, settings_warm);
    }
}

std::generator<Solver> get_hpipm_solvers(const Options &opts) {
    if (opts.hpipm) {
#if WITH_HPIPM
        if (opts.cold)
            co_yield {"hpipm(zero)", [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark_hpipm(state, params, WarmStartHPIPM::WarmZero);
                      }};
        if (opts.warm_shift)
            co_yield {"hpipm(shift)", [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark_hpipm(state, params, WarmStartHPIPM::WarmShift);
                      }};
        if (opts.warm_copy)
            co_yield {"hpipm(copy)", [=](benchmark::State &state, const SpringMassParams &params) {
                          run_benchmark_hpipm(state, params, WarmStartHPIPM::WarmCopy);
                      }};
#else
        throw std::invalid_argument("HPIPM support not enabled in this build");
#endif
    }
}

using std::ranges::elements_of;

template <qp::StorageOrder Order>
std::generator<Solver> get_cyqlone_solvers_vl(const Options &opts) {
    if (opts.vector_length == 0)
        co_return;
#define CYQ_X(VL)                                                                                  \
    else if (opts.vector_length == VL) co_yield elements_of(get_cyqlone_solvers<VL, Order>(opts));
    BATMAT_FOREACH_VL_DOUBLE(CYQ_X)
#undef CYQ_X
    else
#define CYQ_X(VL) " " #VL
        throw std::invalid_argument(
            "Unsupported vector length. Supported lengths:" BATMAT_FOREACH_VL_DOUBLE(CYQ_X));
#undef CYQ_X
}

std::generator<Solver> get_solvers(const Options &opts) {
    co_yield elements_of(get_hpipm_solvers(opts));
    if (opts.rm)
        co_yield elements_of(get_cyqlone_solvers_vl<qp::StorageOrder::RowMajor>(opts));
    if (opts.cm)
        co_yield elements_of(get_cyqlone_solvers_vl<qp::StorageOrder::ColMajor>(opts));
}

auto register_benchmarks(const Options &opts) {
    size_t max_problem_name_len = 0;
    size_t max_solver_name_len  = 0;
    for (auto problem : get_spring_mass_params(opts)) {
        max_problem_name_len = std::max(max_problem_name_len, problem.name.size());
        for (auto solver : get_solvers(opts)) {
            max_solver_name_len = std::max(max_solver_name_len, solver.name.size());
            benchmark::RegisterBenchmark(std::format("{}@{}", problem.name, solver.name),
                                         [params = problem.params, run = std::move(solver.run)](
                                             benchmark::State &state) { run(state, params); })
                ->MeasureProcessCPUTime()
                ->UseManualTime()
                ->Unit(benchmark::kMillisecond)
                ->ComputeStatistics("max", [](auto &v) { return *std::ranges::max_element(v); })
                ->ComputeStatistics("min", [](auto &v) { return *std::ranges::min_element(v); })
                ->GetName();
        }
    }
    return std::make_pair(max_problem_name_len, max_solver_name_len);
}

std::unique_ptr<benchmark::BenchmarkReporter> make_custom_reporter(size_t problem_name_width,
                                                                   size_t solver_name_width,
                                                                   bool print_extra,
                                                                   bool with_color);

void register_options(const char *program, CLI::App &app, Options &opts) {
    if (std::getenv("NO_COLOR"))
        opts.use_color = false;
    else if (std::getenv("CLICOLOR_FORCE"))
        opts.use_color = true;
    else
        opts.use_color = isatty(fileno(stdout)) == 1;
    app.usage(std::string(program) + " [options] -- [benchmark options]");
    app.footer(std::format("Benchmark options are passed to the Google Benchmark framework. "
                           "Use {} -- --help or see "
                           "https://google.github.io/benchmark/user_guide.html for details.",
                           program));
    app.allow_extras();
    app.add_flag("--cold,!--no-cold", opts.cold, "Benchmark with cold starting");
    app.add_flag("--warm-shift,!--no-warm-shift", opts.warm_shift,
                 "Benchmark with warm starting (shift)");
    app.add_flag("--warm-copy,!--no-warm-copy", opts.warm_copy,
                 "Benchmark with warm starting (copy)");
    app.add_option("--horizon,-N", opts.horizon, "Specify a horizon length to benchmark");
    app.add_option("--masses,-M", opts.masses, "Specify the number of masses to benchmark");
    app.add_option("--num-instances,-I", opts.num_instances,
                   "Number of random problem instances to generate for each (M,N) pair");
    app.add_option("--seed,-s", opts.seed, "Random seed for problem instance generation");
    app.add_flag("--no-updates", opts.no_updates, "Compare to the Cyqlone backend without updates");
    app.add_option("--parallelism,-p", opts.parallelism,
                   "The number of threads to use in the Cyqlone backend");
    app.add_option("--vector-length,-v", opts.vector_length,
                   "The vector length to use in the Cyqlone backend");
    app.add_flag("--rm,!--no-rm", opts.rm, "Use row-major (default) storage");
    app.add_flag("--cm,!--no-cm", opts.cm, "Use column-major storage");
    app.add_flag("--pcr,!--pcg", opts.pcr, "Use parallel cyclic reduction in the Cyqlone backend");
    app.add_flag("--hpipm,!--no-hpipm", opts.hpipm, "Use HPIPM solver for comparison");
    app.add_option("--problem", opts.problem_type, "Problem type to benchmark")
        ->transform(CLI::CheckedTransformer(problem_type_map, CLI::ignore_case));
    app.add_option("--pcr-max-update-fraction", opts.pcr_max_update_fraction,
                   "Maximum update rank fraction when using PCR");
    app.add_option("--cr-max-update-fraction", opts.cr_max_update_fraction,
                   "Maximum update rank fraction when using CR");
    app.add_option("--changing-constr-factor", opts.changing_constr_factor,
                   "Changing constraints factor for the Cyqlone backend");
    app.add_option("--parallel-solve-cr-threshold", opts.parallel_solve_cr_threshold,
                   "Parallel CR solve threshold for the Cyqlone backend");
    app.add_option("--parallel-factor-pcr-threshold", opts.parallel_factor_pcr_threshold,
                   "Parallel PCR factorization threshold for the Cyqlone backend");
    app.add_option("--export-problem", opts.export_problem,
                   "Export a single problem instance to a .mat file");
    app.add_flag("--custom-reporter,!--no-custom-reporter", opts.custom_reporter,
                 "Use custom benchmark reporter");
    app.add_flag("--print-extra,!--no-print-extra", opts.print_extra,
                 "Print additional counters in the benchmark report");
    app.add_flag("--color,!--no-color", opts.use_color, "Enable/disable colored output");
}

int initialize_google_benchmark(char *program, auto bm_args) {
    if (!bm_args.empty() && bm_args.front() == "--")
        bm_args.erase(bm_args.begin());
    std::vector<char *> bm_argv(bm_args.size() + 2);
    std::ranges::transform(bm_args, bm_argv.begin() + 1, [](auto &s) { return s.data(); });
    bm_argv[0]   = program;
    auto bm_argc = static_cast<int>(bm_args.size()) + 1;
    benchmark::Initialize(&bm_argc, bm_argv.data());
    if (benchmark::ReportUnrecognizedArguments(bm_argc, bm_argv.data()))
        return 1;
    return 0;
}

void register_context() {
#if BATMAT_WITH_OPENMP
    benchmark::AddCustomContext("OMP_NUM_THREADS", std::to_string(omp_get_max_threads()));
#endif
    benchmark::AddCustomContext("batmat_build_time", batmat_build_time);
    benchmark::AddCustomContext("batmat_commit_hash", batmat_commit_hash);
    benchmark::AddCustomContext("cyqlone_build_time", cyqlone_build_time);
    benchmark::AddCustomContext("cyqlone_commit_hash", cyqlone_commit_hash);
#if defined(__INTEL_LLVM_COMPILER)
    benchmark::AddCustomContext("compiler", "intel-llvm");
#elif defined(__clang__)
    benchmark::AddCustomContext("compiler", "clang");
#elif defined(__GNUC__)
    benchmark::AddCustomContext("compiler", "gcc");
#elif defined(_MSC_VER)
    benchmark::AddCustomContext("compiler", "msvc");
#endif
#if defined(__AVX512F__)
    benchmark::AddCustomContext("arch", "avx512f");
#elif defined(__AVX2__)
    benchmark::AddCustomContext("arch", "avx2");
#elif defined(__AVX__)
    benchmark::AddCustomContext("arch", "avx");
#elif defined(__SSE3__)
    benchmark::AddCustomContext("arch", "sse3");
#elif defined(__ARM_NEON)
    benchmark::AddCustomContext("arch", "neon");
#endif
}

int main(int argc, char **argv) try {
    char *const program = argv[0];
    CLI::App app{"CyQPALM and Cyqlone Spring-Mass Benchmarks"};
    Options opts;
    register_options(program, app, opts);
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    export_problem(opts);
    auto [problem_name_width, solver_name_width] = register_benchmarks(opts);
    if (auto err = initialize_google_benchmark(program, app.remaining(true)); err != 0)
        return err;
    register_context();
    auto reporter = opts.custom_reporter
                        ? make_custom_reporter(problem_name_width, solver_name_width + 2,
                                               opts.print_extra, opts.use_color)
                        : nullptr;
    benchmark::RunSpecifiedBenchmarks(reporter.get());
    benchmark::Shutdown();
} catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
