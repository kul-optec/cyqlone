#include <cyqlone/cyqlone.hpp>
#include <cyqlone/packing.hpp>
#include <batmat/matrix/matrix.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <guanaqo/print.hpp>
#include <algorithm>
#include <iostream>
#include <random>
#include <string>

#if CYQLONE_WITH_MATIO
#include <cyqlone/matio.hpp>
#endif

using cyqlone::index_t;                                   // Integer type for indices
using cyqlone::real_t;                                    // Floating-point type for scalars
using matrices = batmat::matrix::Matrix<real_t, index_t>; // Array of matrices

struct TridiagSystem {
    matrices M, K, b; // Diagonal blocks M, subdiagonal blocks K and right-hand side b
};

// Generate a random SPD block tridiagonal system with given block size and number of blocks.
TridiagSystem init_random_system(index_t block_size, index_t num_blocks, bool circular = false);

// Example of solving a block tridiagonal system using the TricyqleSolver.
int main(int argc, char *argv[]) try {
    // Problem dimensions and parameters
    constexpr index_t v = 4;     // Vector length
    index_t p           = 8;     // Number of processors/threads
    index_t block_size  = 5;     // Size of each block of the block tridiagonal system
    bool circular       = false; // Is K(num_blocks-1) nonzero?
    // Parse arguments
    if (argc > 1)
        p = std::stoi(argv[1]);
    if (argc > 2)
        block_size = std::stoi(argv[2]);
    if (argc > 3)
        circular = std::stoi(argv[3]) != 0;
    // The number of blocks in the block tridiagonal system always equals p * v. If your system is
    // larger, first reduce it to this size (in parallel). If it is smaller, manually add padding,
    // or reduce p and/or v.
    BATMAT_ASSERT(cyqlone::is_pow_2(p));
    const index_t num_blocks = p * v;

    // Diagonal blocks M, subdiagonal blocks K and right-hand side b of a block tridiagonal system
    auto [M, K, b] = init_random_system(block_size, num_blocks, circular);

    // Create a solver for the block tridiagonal system
    using Solver = cyqlone::TricyqleSolver<v, real_t>;
    Solver solver{.block_size = block_size, .circular = circular, .p = p};
    solver.params.solve_method = cyqlone::SolveMethod::PCR; // Use the PCR solver instead of PCG.

    // Create a compact copy of the right-hand side and allocate storage for the solution
    Solver::matrix b_solve{{.depth = num_blocks, .rows = block_size, .cols = 1}};
    matrices x{{.depth = num_blocks, .rows = block_size, .cols = 1}};

    // Call the solver's parallel run method, passing a lambda that is executed by each thread.
    solver.run([&](Solver::Context &ctx) {
        // Copy the data to the solver's internal data structures
        using cyqlone::linalg::pack;
        auto pack_M = [&](index_t i, auto Ms) { return pack(M.middle_batches(i, v, p), Ms); };
        auto pack_K = [&](index_t i, auto Ks) { return pack(K.middle_batches(i, v, p), Ks); };
        auto pack_b = [&](index_t i, auto bs) { return pack(b.middle_batches(i, v, p), bs); };
        solver.init_diag(ctx, pack_M);
        solver.init_subdiag(ctx, pack_K);
        solver.init_rhs(ctx, b_solve, pack_b);
        // Perform the factorization and the forward solve (fused), followed by the backward solve
        solver.factor_solve(ctx, b_solve);
        solver.solve_reverse(ctx, b_solve);
        // Copy the solution back to the original layout
        using cyqlone::linalg::unpack;
        auto unpack_x = [&](index_t i, auto xs) { return unpack(xs, x.middle_batches(i, v, p)); };
        solver.get_solution(ctx, b_solve, unpack_x);
    }); // blocks until all threads have joined

#if CYQLONE_WITH_MATIO
    // Export the original system and the solution as a .mat file
    std::filesystem::path filename = "block_tridiagonal_system.mat";
    auto matfile                   = cyqlone::create_mat(filename);
    cyqlone::add_to_mat(matfile.get(), "M", M);
    cyqlone::add_to_mat(matfile.get(), "K", K);
    cyqlone::add_to_mat(matfile.get(), "b", b);
    cyqlone::add_to_mat(matfile.get(), "x", x);
    std::cout << "Saved system and solution to " << filename << "\n";
#else
    // Print the original system and the solution in a format that's easy to check using Python
    std::cout << "import numpy as np\n";
    std::cout << "v = " << v << "\n";
    std::cout << "n = " << block_size << "\n";
    std::cout << "N = " << num_blocks << "\n";
    std::cout << "M = np.array([\n";
    for (index_t i = 0; i < num_blocks; ++i)
        guanaqo::print_python(std::cout, M(i), ",\n", false);
    std::cout << "])\nK = np.array([\n";
    for (index_t i = 0; i < num_blocks; ++i)
        guanaqo::print_python(std::cout, K(i), ",\n", false);
    std::cout << "])\nb = np.array([\n";
    for (index_t i = 0; i < num_blocks; ++i)
        guanaqo::print_python(std::cout, b(i), ",\n");
    std::cout << "])\nx = np.array([\n";
    for (index_t i = 0; i < num_blocks; ++i)
        guanaqo::print_python(std::cout, x(i), ",\n");
    std::cout << "])\n";
#endif
} catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}

TridiagSystem init_random_system(index_t block_size, index_t num_blocks, bool circular) {
    using batmat::matrix::uninitialized;
    std::mt19937 rng(12345);
    std::normal_distribution<real_t> dist(0.0, 1.0);
    // Generate a random lower block bidiagonal matrix with diagonal blocks A(i) and subdiagonal
    // blocks B(i).
    matrices A{{.depth = num_blocks, .rows = block_size, .cols = block_size}, uninitialized};
    matrices B{{.depth = num_blocks, .rows = block_size, .cols = block_size}, uninitialized};
    std::ranges::generate(A, [&] { return dist(rng); });
    std::ranges::generate(B, [&] { return dist(rng); });
    // Allocate storage for a block tridiagonal system
    matrices M{{.depth = num_blocks, .rows = block_size, .cols = block_size}, uninitialized};
    matrices K{{.depth = num_blocks, .rows = block_size, .cols = block_size}, uninitialized};
    matrices b{{.depth = num_blocks, .rows = block_size, .cols = 1}, uninitialized};
    std::ranges::generate(b, [&] { return dist(rng); });
    if (!circular)
        B(num_blocks - 1).set_constant(0);
    // Diagonal blocks M(i) = A(i) A(i)ᵀ + B(i-1) B(i-1)ᵀ and subdiagonal blocks K(i) = B(i) A(i)ᵀ
    for (index_t i = 0; i < num_blocks; ++i) {
        index_t i_prev = (i - 1 + num_blocks) % num_blocks;
        guanaqo::blas::xsyrk_LN<real_t>(1, A(i), 0, M(i));       // M(i) = A(i) A(i)ᵀ
        guanaqo::blas::xsyrk_LN<real_t>(1, B(i_prev), 1, M(i));  // M(i) += B(i-1) B(i-1)ᵀ
        guanaqo::blas::xgemm_NT<real_t>(1, B(i), A(i), 0, K(i)); // K(i) = B(i) A(i)ᵀ
        M(i).add_to_diagonal(1e-4);                              // Ensure positive definiteness
    }
    return TridiagSystem{.M = std::move(M), .K = std::move(K), .b = std::move(b)};
}
