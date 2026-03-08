#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>
#include <cyqlone/ocp.hpp>
#include <batmat/dtypes.hpp>
#include <guanaqo/print.hpp>
#include <iostream>
#include <random>
#include <string>

#if CYQLONE_WITH_MATIO
#include <cyqlone/matio.hpp>
#endif

namespace cq = cyqlone;
using cq::index_t;    // Integer type for indices
using cq::real_t;     // Floating-point type for scalars
constexpr index_t v = // Vector length to use
    batmat::types::vl_at_most<real_t, 4>;

// Generate a random linear-quadratic OCP.
cq::LinearOCPStorage init_ocp(index_t N, index_t nx, index_t nu) {
    using batmat::linalg::tril;
    using batmat::linalg::triu;

    std::mt19937 rng{12345};
    std::uniform_real_distribution<real_t> uni_dist{-1.2, 1.2};
    auto uni_gen = [&] { return uni_dist(rng); };

    cq::LinearOCPStorage ocp{.dim{.N_horiz = N, .nx = nx, .nu = nu, .ny = 0}};
    auto diag = 2 * static_cast<real_t>(nx + nu);
    ocp.b(0).generate(uni_gen); // initial state x(0)
    for (index_t j = 0; j < N; ++j) {
        ocp.A(j).generate(uni_gen);     // Dynamics Jacobian df/dx
        ocp.B(j).generate(uni_gen);     // Dynamics Jacobian df/du
        ocp.b(j + 1).generate(uni_gen); // Dynamics constant term f(j)
        ocp.Q(j).generate(uni_gen);     // Cost Hessian d²ℓ/dx²
        ocp.R(j).generate(uni_gen);     // Cost Hessian d²ℓ/du²
        ocp.S(j).generate(uni_gen);     // Cost Hessian d²ℓ/dxdu
        ocp.q(j).generate(uni_gen);     // Cost gradient dℓ/dx
        ocp.r(j).generate(uni_gen);     // Cost gradient dℓ/du
        ocp.Q(j).add_to_diagonal(diag); // ensure positive definiteness
        ocp.R(j).add_to_diagonal(diag);
        copy(tril(ocp.H(j)).transposed(), triu(ocp.H(j))); // ensure symmetry of cost Hessian
    }
    ocp.Q(N).generate(uni_gen); // Terminal cost Hessian d²ℓ_N(x)/dx²
    ocp.q(N).generate(uni_gen);
    ocp.Q(N).add_to_diagonal(diag);
    copy(tril(ocp.Q(N)).transposed(), triu(ocp.Q(N)));
    return ocp;
}

// Example of solving a linear-quadratic OCP using the CyqloneSolver.
int main(int argc, char *argv[]) try {
    // Problem dimensions and parameters
    const index_t p  = argc > 1 ? std::stoi(argv[1]) : 8;   // Number of processors/threads
    const index_t N  = argc > 2 ? std::stoi(argv[2]) : 128; // Horizon length
    const index_t nx = argc > 3 ? std::stoi(argv[3]) : 10;  // State dimension
    const index_t nu = argc > 4 ? std::stoi(argv[4]) : 5;   // Control dimension
    BATMAT_ASSERT(v == 1 || cq::is_pow_2(p));

    // Initialize a random linear-quadratic OCP
    auto ocp = init_ocp(N, nx, nu);
    // Eliminate the initial state x0 and merge the first and last stages for Cyqlone
    auto cocp = cq::CyqloneStorage<real_t>::build(ocp);

    // Create a Cyqlone solver for the OCP
    cq::CyqloneSolver<v, real_t> solver = cq::CyqloneSolver<v, real_t>::build(cocp, p);
    // Optionally configure the solver parameters
    auto params         = solver.get_tricyqle_params();
    params.solve_method = cq::SolveMethod::PCR; // Use the PCR solver instead of PCG
    solver.update_tricyqle_params(params);
    std::cout << solver.get_params_string() << "\n";

    // Initialize copies of the OCP data vectors in compact storage format
    const auto rq = solver.initialize_gradient(cocp); // gradient vectors r and q
    const auto b  = solver.initialize_rhs(cocp); // right-hand side vector b of dynamics constraints
    // Initialize the right-hand side of the KKT system (will be overwritten by the solution)
    auto ux = solver.initialize_variables(), λ = solver.initialize_dynamics_constraints();
    cq::linalg::negate(rq, ux); // right-hand side of the KKT system is negative gradient
    cq::linalg::copy(b, λ);
    const real_t γ = 1e99; // Primal regularization

    // Call the solver in parallel, passing a lambda that is executed by each thread
    auto pctx = solver.create_parallel_context();
    auto t0   = std::chrono::high_resolution_clock::now();
    pctx->run([&](cq::CyqloneSolver<v, real_t>::Context &ctx) {
        solver.factor_solve(ctx, γ, {}, ux, λ); // Cholesky factorization and forward substitution
        solver.solve_reverse(ctx, ux, λ);       // Backward substitution
    });                                         // blocks until all threads have joined
    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration<double, std::milli>(t1 - t0).count() << " ms\n";

    // Compute the residuals (in compact storage and without x0, also in parallel)
    auto Mxb  = solver.initialize_dynamics_constraints(); // primal equality constraint residual
    auto grad = solver.initialize_variables();            // gradient of the Lagrangian
    pctx->run([&](cq::CyqloneSolver<v, real_t>::Context &ctx) {
        solver.residual_dynamics_constr(ctx, ux, b, Mxb); // Mx + b
        solver.transposed_dynamics_constr(ctx, λ, grad);  // Mᵀλ
        solver.cost_gradient(ctx, ux, 1, rq, 1, grad);    // + Qx + q
    });

    // Print the residuals
    using cq::linalg::norm_inf;
    std::cout << "Eq. residual (compact): " << guanaqo::float_to_str(norm_inf(Mxb)) << "\n"
              << "Lagr. stationarity (compact): " << guanaqo::float_to_str(norm_inf(grad)) << "\n";

    // Reconstruct the full solution (as a flat vector, including x0)
    std::vector ux_unpacked = solver.unpack_variables(ux); // convert compact to linear storage
    std::vector λ_unpacked  = solver.unpack_dynamics(λ);
    auto sol                = cocp.reconstruct_solution(ocp, ux_unpacked, {}, λ_unpacked);

    // Check the residuals of the original OCP
    auto resid = ocp.compute_kkt_error(sol);
    std::cout << "Eq. residual (full): " << guanaqo::float_to_str(resid.equality_residual) << "\n"
              << "Lagr. stationarity (full): " << guanaqo::float_to_str(resid.stationarity) << "\n";

#if CYQLONE_WITH_MATIO
    // Export the original OCP and the solution as a .mat file
    std::filesystem::path filename = "ocp_solution.mat";
    auto matfile                   = cq::create_mat(filename);
    cq::add_to_mat(matfile.get(), "ocp", ocp);
    cq::add_to_mat(matfile.get(), "sol_x", sol.solution);
    cq::add_to_mat(matfile.get(), "sol_λ", sol.equality_multipliers);
    std::cout << "Saved OCP and solution to " << filename << "\n";
#endif
} catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}
