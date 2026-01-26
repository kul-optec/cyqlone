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
    using Solver     = v2::CyqloneSolver<1, real_t, v2::StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::vl, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.m_update, index_t{1});

    EXPECT_EQ(solver.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 3), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 5), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 5), std::make_pair(5, 6));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 7), std::make_pair(6, 7));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 7), std::make_pair(7, 8));

    EXPECT_EQ(solver.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 6), std::make_pair(5, 6));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 6), std::make_pair(6, 7));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 0), std::make_pair(7, 8));

    EXPECT_EQ(solver.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 2), std::make_pair(2, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 6), std::make_pair(4, 6));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 6), std::make_pair(6, 8));

    EXPECT_EQ(solver.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 4), std::make_pair(4, 6));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 0), std::make_pair(6, 8));
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
    using Solver     = v2::CyqloneSolver<1, real_t, v2::StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::vl, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.m_update, index_t{1});

    EXPECT_EQ(solver.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 3), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 5), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 5), std::make_pair(5, 6));

    EXPECT_EQ(solver.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 6), std::make_pair(5, 6));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 6), std::make_pair(6, 7));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 0), std::make_pair(6, 7));

    EXPECT_EQ(solver.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 2), std::make_pair(2, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 6), std::make_pair(4, 6));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 6), std::make_pair(6, 7));

    EXPECT_EQ(solver.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 4), std::make_pair(4, 6));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 0), std::make_pair(6, 7));

    EXPECT_EQ(solver.cols_Ups_fwd(2, 4), std::make_pair(0, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(2, 4), std::make_pair(4, 7));

    EXPECT_EQ(solver.cols_Ups_bwd(2, 0), std::make_pair(0, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(2, 0), std::make_pair(4, 7));
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
    using Solver     = v2::CyqloneSolver<1, real_t, v2::StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::vl, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.m_update, index_t{1});

    EXPECT_EQ(solver.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 3), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 5), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 5), std::make_pair(5, 6));

    EXPECT_EQ(solver.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 0), std::make_pair(5, 6));

    EXPECT_EQ(solver.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 2), std::make_pair(2, 4));

    EXPECT_EQ(solver.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 4), std::make_pair(4, 6));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 0), std::make_pair(4, 6));

    EXPECT_EQ(solver.cols_Ups_fwd(2, 4), std::make_pair(0, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(2, 4), std::make_pair(4, 6));

    EXPECT_EQ(solver.cols_Ups_bwd(2, 0), std::make_pair(0, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(2, 0), std::make_pair(4, 6));
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
    using Solver     = v2::CyqloneSolver<1, real_t, v2::StorageOrder::RowMajor>;
    const index_t ny = 1, ny_0 = 1, ny_N = 0;
    OCPDim dim{.N_horiz = p * Solver::vl, .nx = 1, .nu = 1, .ny = ny, .ny_N = ny_N};
    auto ocp = generate_random_ocp(dim);
    ocp.D(0).bottom_rows(ny - ny_0).set_constant(0);
    auto cocp     = CyqloneStorage<real_t>::build(ocp);
    Solver solver = Solver::build(cocp, p);
    std::ranges::iota(solver.m_update, index_t{1});

    EXPECT_EQ(solver.cols_Ups_fwd(0, 1), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 1), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 3), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 3), std::make_pair(3, 4));

    EXPECT_EQ(solver.cols_Ups_bwd(0, 0), std::make_pair(0, 1));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 2), std::make_pair(1, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 2), std::make_pair(2, 3));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 4), std::make_pair(3, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(0, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_fwd(0, 0), std::make_pair(4, 5));

    EXPECT_EQ(solver.cols_Ups_fwd(1, 2), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 2), std::make_pair(2, 4));

    EXPECT_EQ(solver.cols_Ups_bwd(1, 0), std::make_pair(0, 2));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 4), std::make_pair(2, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(1, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_fwd(1, 0), std::make_pair(4, 5));

    EXPECT_EQ(solver.cols_Ups_fwd(2, 4), std::make_pair(0, 4));
    EXPECT_EQ(solver.cols_Ups_bwd(2, 4), std::make_pair(4, 5));
    EXPECT_EQ(solver.cols_Ups_bwd(2, 0), std::make_pair(0, 4));
    EXPECT_EQ(solver.cols_Ups_fwd(2, 0), std::make_pair(4, 5));
}

TEST_P(CyqloneFactorTest, factor) {
    using namespace cyqlone;
    using batmat::linalg::simdify;

    const index_t p  = 8;
    using Solver     = v2::CyqloneSolver<4, real_t, v2::StorageOrder::RowMajor>;
    const index_t ny = 50, ny_0 = 25, ny_N = 25, nyM = std::max(ny, ny_0 + ny_N);
    OCPDim dim{.N_horiz = 97, .nx = 40, .nu = 30, .ny = ny, .ny_N = ny_N};
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
    auto cocp           = CyqloneStorage<real_t>::build(ocp);
    Solver solver       = Solver::build(cocp, p);
    solver.solve_method = GetParam();

    // Spin a bit longer to get more deterministic timings
    solver.parallel_ctx->barrier.spin_count = std::numeric_limits<uint32_t>::max();

    GUANAQO_IF_ITT(solver.parallel_ctx->run(
        [](auto &ctx) { __itt_thread_set_name(std::format("OMP({})", ctx.index).c_str()); }));

    std::vector<real_t> Σ_lin((N - 1) * ny + ny_0 + ny_N);
    Solver::matrix λ{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        ux{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        Mᵀλ{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        DCux{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        DCᵀΣDCux{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        grad{{.depth = solver.ceil_N(), .rows = nux, .cols = 1}},
        Mxb{{.depth = solver.ceil_N(), .rows = dim.nx, .cols = 1}},
        Σ{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        Σ2{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}},
        ΔΣ{{.depth = solver.ceil_N(), .rows = nyM, .cols = 1}};
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
