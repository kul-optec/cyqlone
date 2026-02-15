#include <cyqlone/cyqlone.hpp>
#include <cyqlone-version.h>
#if WITH_QPALM
#include <cyqlone/qpalm/backends/ocp-backend-cyqlone.hpp>
#include <cyqlone/qpalm/example-problems/platooning.hpp>
#include <cyqlone/qpalm/qpalm.hpp>
#endif

#include <iostream>
#include <syncstream>

int main() {
    std::cout << "\ncyqlone " << CYQLONE_VERSION << " (" << cyqlone_build_time << ":"
              << cyqlone_commit_hash << ")\n"
              << std::endl;
#if WITH_QPALM
    namespace qp       = cyqlone::qpalm;
    auto ocp           = qp::problems::platooning({.N_horiz = 32});
    auto cocp          = cyqlone::CyqloneStorage<>::build(ocp.ocp);
    const bool verbose = true;
    qp::CyQPALMBackendSettings params{.processors      = 4,
                                      .print_residuals = verbose,
                                      .print_precision = 17,
                                      .tricyqle_params{.solve_method = cyqlone::SolveMethod::PCR}};
    qp::Settings qpalm_params{.max_outer_iter       = 400,
                              .max_total_inner_iter = 400,
                              .tolerance            = 1e-8,
                              .dual_tolerance       = 1e-8,
                              .max_penalty_y        = 1e7,
                              .initial_penalty_y    = 1e-2,
                              .verbose              = verbose};
    auto backend = qp::make_cyqpalm_backend<4>(cocp, {}, params);
    qp::Solver<qp::CyQPALMBackend<4> *> cyqpalm{backend.get(), qpalm_params};
    auto status = cyqpalm();
    std::cout << "CyQPALM status: " << enum_name(status) << std::endl;
    return status == qp::SolverStatus::Converged ? 0 : 1;
#else
    cyqlone::TricyqleSolver<4> solver{.block_size = 42, .max_rank = 10, .p = 4};
    solver.run([](auto &ctx) {
        std::osyncstream(std::cout) << "Hello from thread " << ctx.index << "!" << std::endl;
    });
#endif
}
