#include <gtest/gtest.h>

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone.hpp>
#include <cyqlone/linalg.hpp>
#include <cyqlone/packing.hpp>
#include <batmat/config.hpp>
#include <batmat/matrix/matrix.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <guanaqo/demangled-typename.hpp>
#include <algorithm>
#include <cmath>
#include <format>
#include <random>

using cyqlone::index_t;

namespace {

template <class T>
struct TridiagSystem {
    using real_t   = T;
    using matrices = batmat::matrix::Matrix<real_t, index_t>;
    matrices M, K, b; // Diagonal blocks M, subdiagonal blocks K and right-hand side b
};

// Generate a random SPD block tridiagonal system with given block size and number of blocks.
template <class T>
auto init_random_system(index_t block_size, index_t num_blocks, bool circular) {
    using real_t   = T;
    using matrices = batmat::matrix::Matrix<real_t, index_t>;
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
        M(i).add_to_diagonal(1e-2);                              // Ensure positive definiteness
    }
    return TridiagSystem<T>{.M = std::move(M), .K = std::move(K), .b = std::move(b)};
}

// Compute the L2 norm of the residual r = Ax - b for a block tridiagonal system.
template <class T>
T compute_residual_norm(const TridiagSystem<T> &sys, const typename TridiagSystem<T>::matrices &x,
                        bool circular) {
    using real_t   = T;
    using matrices = batmat::matrix::Matrix<real_t, index_t>;
    using batmat::matrix::uninitialized;
    using std::sqrt;

    index_t num_blocks = sys.M.depth(), block_size = sys.M.rows();
    // Compute residual for each block: r = Ax - b
    matrices r{{.depth = num_blocks, .rows = block_size, .cols = 1}, uninitialized};
    real_t norm_sq_r = 0, norm_sq_b = 0;
    for (index_t i = 0; i < num_blocks; ++i) {
        const index_t i_prev = (i - 1 + num_blocks) % num_blocks, i_next = (i + 1) % num_blocks;
        r(i) = sys.b(i);
        guanaqo::blas::xsymv_L<real_t>(1, sys.M(i), x(i), -1, r(i));
        if (i > 0 || circular)
            guanaqo::blas::xgemv_N<real_t>(1, sys.K(i_prev), x(i_prev), 1, r(i));
        if (i < num_blocks - 1 || circular)
            guanaqo::blas::xgemv_T<real_t>(1, sys.K(i), x(i_next), 1, r(i));
        norm_sq_r += cyqlone::linalg::norm_2_squared(r(i));
        norm_sq_b += cyqlone::linalg::norm_2_squared(sys.b(i));
    }
    return sqrt(norm_sq_r / norm_sq_b);
}

template <class Solver>
[[gnu::noinline]] void test_tricyqle(index_t block_size, index_t num_threads, bool circular,
                                     cyqlone::SolveMethod method) {
    using real_t   = typename Solver::value_type;
    using matrices = batmat::matrix::Matrix<real_t, index_t>;

    constexpr index_t v      = Solver::v;
    const index_t p          = num_threads;
    const index_t num_blocks = v * p;

    if (v == 1 && circular && method == cyqlone::SolveMethod::PCR) // TODO
        GTEST_SKIP() << "PCR does not yet support circular systems in the scalar case (v=1).";

    // Generate random block tridiagonal system
    auto sys = init_random_system<real_t>(block_size, num_blocks, circular);

    // Create solver
    Solver solver{.block_size = block_size, .circular = circular, .p = num_threads};
    solver.params.solve_method = method;
    solver.params.pcg_max_iter = block_size * num_blocks + 10;

    // Allocate storage for solution
    typename Solver::matrix b_solve{{.depth = num_blocks, .rows = block_size, .cols = 1}};
    matrices x{{.depth = num_blocks, .rows = block_size, .cols = 1}};

    // Solve the system
    solver.run([&](typename Solver::Context &ctx) {
        using cyqlone::linalg::pack;
        using cyqlone::linalg::unpack;

        auto pack_M = [&](index_t i, auto Ms) { return pack(sys.M.middle_batches(i, v, p), Ms); };
        auto pack_K = [&](index_t i, auto Ks) { return pack(sys.K.middle_batches(i, v, p), Ks); };
        auto pack_b = [&](index_t i, auto bs) { return pack(sys.b.middle_batches(i, v, p), bs); };
        solver.init_diag(ctx, pack_M);
        solver.init_subdiag(ctx, pack_K);
        solver.init_rhs(ctx, b_solve, pack_b);

        solver.factor_solve(ctx, b_solve);
        solver.solve_reverse(ctx, b_solve);

        auto unpack_x = [&](index_t i, auto xs) { return unpack(xs, x.middle_batches(i, v, p)); };
        solver.get_solution(ctx, b_solve, unpack_x);
    });

    // Check that the residual is small
    using std::pow;
    const auto tol  = pow(std::numeric_limits<real_t>::epsilon(), real_t(0.5));
    real_t residual = compute_residual_norm(sys, x, circular);
    EXPECT_LT(residual, tol) << "Relative residual too large: " << residual;
}

} // namespace

// Test configuration
template <class T, index_t VL, cyqlone::StorageOrder O, index_t BS, index_t P, bool Circ,
          cyqlone::SolveMethod Method>
struct TestConfig {
    using real_t                                       = T;
    static constexpr index_t v                         = VL;
    static constexpr cyqlone::StorageOrder order       = O;
    static constexpr index_t block_size                = BS;
    static constexpr index_t num_threads               = P;
    static constexpr bool circular                     = Circ;
    static constexpr cyqlone::SolveMethod solve_method = Method;
};

// Parametrized test fixture
template <class Config>
class TricyqleTest : public ::testing::Test {
  protected:
    using Solver = cyqlone::TricyqleSolver<Config::v, typename Config::real_t, Config::order>;
};

// Generate test type combinations using X-macro expansion
// Strategy: Create cartesian product of all parameter dimensions using nested macro expansion
namespace {

template <class Types>
struct StripFirstType; // Helper to strip the first type from a Types<> list
template <class First, class... Rest>
struct StripFirstType<::testing::Types<First, Rest...>> {
    using type = ::testing::Types<Rest...>;
};

// Define parameter ranges for each dimension
#define TRICYQLE_TEST_STORAGE_ORDERS(X, ...)                                                       \
    X(cyqlone::StorageOrder::RowMajor __VA_OPT__(, ) __VA_ARGS__)                                  \
    X(cyqlone::StorageOrder::ColMajor __VA_OPT__(, ) __VA_ARGS__)
#define TRICYQLE_TEST_BLOCK_SIZES(X, ...)                                                          \
    X(1 __VA_OPT__(, ) __VA_ARGS__)                                                                \
    X(5 __VA_OPT__(, ) __VA_ARGS__)                                                                \
    X(31 __VA_OPT__(, ) __VA_ARGS__)
#define TRICYQLE_TEST_NUM_THREADS(X, ...)                                                          \
    X(1 __VA_OPT__(, ) __VA_ARGS__)                                                                \
    X(2 __VA_OPT__(, ) __VA_ARGS__)                                                                \
    X(4 __VA_OPT__(, ) __VA_ARGS__)                                                                \
    X(32 __VA_OPT__(, ) __VA_ARGS__)
#define TRICYQLE_TEST_CIRCULAR(X, ...)                                                             \
    X(false __VA_OPT__(, ) __VA_ARGS__)                                                            \
    X(true __VA_OPT__(, ) __VA_ARGS__)
#define TRICYQLE_TEST_SOLVE_METHODS(X, ...)                                                        \
    X(cyqlone::SolveMethod::PCR __VA_OPT__(, ) __VA_ARGS__)                                        \
    X(cyqlone::SolveMethod::StairPCG __VA_OPT__(, ) __VA_ARGS__)

// Nested macro expansion to generate cartesian product
// Each level expands the next dimension, building up the parameter list
#define TRICYQLE_TEST_WITH_METHOD(Method, T, V, Order, BlockSize, P, Circ)                         \
    , TestConfig<T, V, Order, BlockSize, P, Circ, Method>
#define TRICYQLE_TEST_WITH_CIRCULAR(Circ, T, V, Order, BlockSize, P)                               \
    TRICYQLE_TEST_SOLVE_METHODS(TRICYQLE_TEST_WITH_METHOD, T, V, Order, BlockSize, P, Circ)
#define TRICYQLE_TEST_WITH_NUM_THREADS(P, T, V, Order, BlockSize)                                  \
    TRICYQLE_TEST_CIRCULAR(TRICYQLE_TEST_WITH_CIRCULAR, T, V, Order, BlockSize, P)
#define TRICYQLE_TEST_WITH_BLOCK_SIZE(BlockSize, T, V, Order)                                      \
    TRICYQLE_TEST_NUM_THREADS(TRICYQLE_TEST_WITH_NUM_THREADS, T, V, Order, BlockSize)
#define TRICYQLE_TEST_WITH_ORDER(Order, T, V)                                                      \
    TRICYQLE_TEST_BLOCK_SIZES(TRICYQLE_TEST_WITH_BLOCK_SIZE, T, V, Order)
#define TRICYQLE_TEST_WITH_DTYPE_VL(T, V)                                                          \
    TRICYQLE_TEST_STORAGE_ORDERS(TRICYQLE_TEST_WITH_ORDER, T, V)

const char *enum_name(cyqlone::StorageOrder o) {
    return o == cyqlone::StorageOrder::ColMajor ? "ColMajor" : "RowMajor";
}

} // namespace

using TricyqleTestTypes = StripFirstType<
    ::testing::Types<void BATMAT_FOREACH_DTYPE_VL(TRICYQLE_TEST_WITH_DTYPE_VL)>>::type;

// Custom name generator for readable test names
struct TricyqleTestNameGenerator {
    template <class Config>
    static std::string GetName(int) {
        return std::format("{}[{}]-{}-n{}-p{}-{}{}",
                           guanaqo::demangled_typename(typeid(typename Config::real_t)), Config::v,
                           enum_name(Config::order), Config::block_size, Config::num_threads,
                           enum_name(Config::solve_method), Config::circular ? "-circular" : "");
    }
};

TYPED_TEST_SUITE(TricyqleTest, TricyqleTestTypes, TricyqleTestNameGenerator);

TYPED_TEST(TricyqleTest, Solve) {
    test_tricyqle<typename TestFixture::Solver>(TypeParam::block_size, TypeParam::num_threads,
                                                TypeParam::circular, TypeParam::solve_method);
}
