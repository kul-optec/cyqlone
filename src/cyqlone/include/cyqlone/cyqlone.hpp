#pragma once

/// @file
/// The main header for the Cyqlone and Tricyqle linear solvers.
/// @ingroup topic-lin-solvers

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone-params.hpp>
#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/parallel.hpp>
#include <cyqlone/sparse.hpp>
#include <cyqlone/timing.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <batmat/linalg/hyhound.hpp> // TODO: isolate size functions
#include <batmat/matrix/layout.hpp>
#include <batmat/matrix/matrix.hpp>
#include <batmat/openmp.h>
#include <batmat/simd.hpp>
#include <batmat/unroll.h>
#include <guanaqo/trace.hpp>

#include <algorithm>
#include <bit>
#include <cassert>
#include <utility>

namespace CYQLONE_NS(cyqlone) {

using batmat::matrix::StorageOrder;

[[nodiscard]] constexpr bool is_pow_2(index_t n) {
    BATMAT_ASSUME(n > 0);
    auto un = static_cast<std::make_unsigned_t<index_t>>(n);
    return std::has_single_bit(un);
}

[[nodiscard]] constexpr index_t ceil_log2(index_t n) {
    BATMAT_ASSUME(n > 0);
    auto un = static_cast<std::make_unsigned_t<index_t>>(n);
    return static_cast<index_t>(std::bit_width(un - 1));
}

// TODO: replace by ν2
[[nodiscard]] constexpr index_t get_level(index_t i) {
    BATMAT_ASSUME(i > 0);
    auto ui = static_cast<std::make_unsigned_t<index_t>>(i);
    return static_cast<index_t>(std::countr_zero(ui));
}
// TODO: move to indexing.tpp or data.tpp?
[[nodiscard]] constexpr index_t get_index_in_level(index_t i) {
    if (i == 0)
        return 0;
    auto l = get_level(i);
    return i >> (l + 1);
}

/// Solver for block-tridiagonal systems using cyclic reduction (CR), parallel cyclic reduction
/// (PCR), and preconditioned conjugate gradient (PCG) methods.
/// @tparam VL              Vector length.
/// @tparam T               Scalar type.
/// @tparam DefaultOrder    Storage order for the matrix workspaces (row/column major).
/// @ingroup topic-block-tridiag-solvers
template <index_t VL = 4, class T = real_t, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct TricyqleSolver {
    using value_type = T;
    using Params     = TricyqleParams<value_type>;

    /// @name Problem dimensions
    /// @{

    const index_t block_size; ///< Block size of the block-tridiagonal system.
    const index_t max_rank;   ///< Maximum update rank.

    /// @}

    /// @name Solver parameters
    /// @{

    /// Solver parameters for Tricyqle-specific settings.
    Params params{};

    /// Get the current solver parameters.
    [[nodiscard]] Params get_params() const { return params; }

    /// Update the solver parameters.
    void update_params(const Params &new_params) { params = new_params; }

    /// @}

    /// @name Parallelization and vectorization
    /// @{

    /// Number of processors/threads.
    const index_t p = 8;
    /// Vector length.
    static constexpr index_t v = VL;
    /// log₂(p), logarithm of the number of processors/threads, rounded up.
    [[nodiscard]] constexpr index_t lp() const { return ceil_log2(p); }
    /// The number of processors @ref p rounded up to the next power of two.
    [[nodiscard]] constexpr index_t ceil_p() const { return 1 << lp(); }
    /// The number of parallel execution units P rounded up to the next power of two.
    [[nodiscard]] constexpr index_t ceil_P() const { return 1 << (lp() + lv()); }
    /// log₂(v), logarithm of the vector length.
    [[nodiscard]] static constexpr index_t lv() { return ceil_log2(v); }

    using simd          = batmat::datapar::deduced_simd<value_type, v>;
    using vl_t          = std::integral_constant<index_t, v>;
    using align_t       = std::integral_constant<index_t, v * alignof(value_type)>;
    using SharedContext = parallel::SharedContext;
    using Context       = parallel::Context<SharedContext>;
    std::unique_ptr<SharedContext> parallel_ctx = std::make_unique<SharedContext>(p);

    /// Run a function in parallel.
    void run(auto &&func) const { return parallel_ctx->run(std::forward<decltype(func)>(func)); }

    /// @}

    /// @name Indexing utilities
    /// @{

    /// 2-adic valuation ν₂.
    [[nodiscard]] index_t ν2(index_t i) const;
    /// 2-adic valuation modulo p, i.e. `ν2p(0) = ν2p(p) = lp()`.
    [[nodiscard]] index_t ν2p(index_t i) const;
    /// Add @p b to @p a modulo @ref ceil_p().
    [[nodiscard]] index_t add_wrap_ceil_p(index_t a, index_t b) const;
    /// Subtract @p b from @p a modulo @ref ceil_p().
    [[nodiscard]] index_t sub_wrap_ceil_p(index_t a, index_t b) const;

    /// @}

    /// @name Matrix data structures
    /// @{

    /// Default storage order for most matrices.
    static constexpr auto default_order = DefaultOrder;
    /// Column-major storage order for column vectors and update matrices.
    static constexpr auto column_major = StorageOrder::ColMajor;

    /// Owning type for a batch of matrices (with batch size v).
    template <StorageOrder O = column_major>
    using matrix = batmat::matrix::Matrix<value_type, index_t, vl_t, index_t, O, align_t>;
    /// Non-owning immutable view type for @ref matrix.
    template <StorageOrder O = column_major>
    using view = batmat::matrix::View<const value_type, index_t, vl_t, index_t, index_t, O>;
    /// Non-owning mutable view type for @ref matrix.
    template <StorageOrder O = column_major>
    using mut_view     = batmat::matrix::View<value_type, index_t, vl_t, index_t, index_t, O>;
    using layer_stride = batmat::matrix::DefaultStride;
    /// Non-owning immutable view type for a single batch of v matrices.
    template <StorageOrder O = column_major>
    using batch_view = batmat::matrix::View<const value_type, index_t, vl_t, vl_t, layer_stride, O>;
    /// Non-owning mutable view type for a single batch of v matrices.
    template <StorageOrder O = column_major>
    using mut_batch_view = batmat::matrix::View<value_type, index_t, vl_t, vl_t, layer_stride, O>;

    /// @}

    /// @name Cyclic reduction data structures
    /// @{

    /// Diagonal blocks of the Cholesky factor of the Schur complement (used during CR).
    matrix<default_order> cr_L = [this] {
        return matrix<default_order>{{.depth = p * v, .rows = block_size, .cols = block_size}};
    }();
    /// Subdiagonal blocks U of the Cholesky factor of the Schur complement (used during CR).
    matrix<default_order> cr_U = [this] {
        return matrix<default_order>{{.depth = p * v, .rows = block_size, .cols = block_size}};
    }();
    /// Subdiagonal blocks Y of the Cholesky factor of the Schur complement (used during CR).
    matrix<default_order> cr_Y = [this] {
        return matrix<default_order>{{.depth = p * v, .rows = block_size, .cols = block_size}};
    }();
    /// Temporary workspace for the CR solve phase.
    matrix<column_major> work_cr = [this] {
        return matrix<column_major>{{.depth = p * v, .rows = block_size, .cols = 1}};
    }();

    /// @}

    /// @name Parallel cyclic reduction data structures
    /// @{

    /// Diagonal blocks of the PCR Cholesky factorizations.
    matrix<default_order> pcr_L = [this] {
        return matrix<default_order>{
            {.depth = v * (lv() + 1), .rows = block_size, .cols = block_size}};
    }();
    /// Subdiagonal blocks Y of the PCR Cholesky factorizations.
    matrix<default_order> pcr_Y = [this] {
        return matrix<default_order>{{.depth = v * lv(), .rows = block_size, .cols = block_size}};
    }();
    /// Subdiagonal blocks U of the PCR Cholesky factorizations.
    matrix<default_order> pcr_U = [this] {
        return matrix<default_order>{{.depth = v * lv(), .rows = block_size, .cols = block_size}};
    }();
    /// Workspace to store the diagonal blocks during the PCR factorization.
    matrix<default_order> pcr_M = [this] {
        return matrix<default_order>{{.depth = v, .rows = block_size, .cols = block_size}};
    }();
    /// Temporary workspace for CG vectors.
    matrix<column_major> work_pcg = [this] {
        return matrix<column_major>{{.depth = v, .rows = block_size, .cols = 4}};
    }();

    /// @}

    /// @name Factorization update data structures
    /// @{

    /// Update rank (number of changing constraints) per thread.
    std::vector<index_t> m_update = std::vector<index_t>(p);
    /// Update rank from D(0). Negative if D(0) is not handled separately.
    index_t m_update_u0 = -1;

    /// Compressed reprentation of the nonzero diagonal elements of the matrix Σ.
    matrix<column_major> work_update_Σ = [this] {
        return matrix<column_major>{{.depth = v, .rows = max_rank, .cols = 1}};
    }();
    /// Workspace to store the update matrices Ξ(Υ) for the factorization update.
    matrix<column_major> work_update = [this] {
        return matrix<column_major>{{.depth = 4 * v, .rows = block_size, .cols = max_rank}};
    }();
    /// Storage for the hyperbolic Householder transformations.
    matrix<column_major> work_hyh = [this] {
        using namespace batmat::linalg;
        const auto [r, c] = hyhound_size_W(tril(cr_L.batch(0)));
        return matrix<column_major>{{.depth = p * v, .rows = r, .cols = c}};
    }();

    /// Two copies of @ref work_update_Σ for PCR updates.
    matrix<column_major> work_update_pcr_Σ = [this] {
        return matrix<column_major>{{.depth = v, .rows = 2 * max_rank, .cols = 1}};
    }();
    /// Update matrices to apply to the diagonal blocks L during PCR updates.
    matrix<column_major> work_update_pcr_L = [this] {
        return matrix<column_major>{{.depth = v, .rows = block_size, .cols = max_rank}};
    }();
    /// Update matrices to apply to the subdiagonal blocks U and Y during PCR updates.
    matrix<column_major> work_update_pcr_UY = [this] {
        return matrix<column_major>{{.depth = v, .rows = block_size, .cols = 2 * max_rank}};
    }();

    /// @}

    /// @name Low-level CR factorization and solve routines
    /// @{

    template <bool Factor, bool Solve>
    void factor_solve_cr(Context &ctx, mut_view<> λ, index_t stride);
    [[nodiscard]] index_t cr_thread_assignment(index_t l, index_t c) const;
    void factor_U(index_t l, index_t iU);
    void factor_Y(index_t l, index_t iY);
    void factor_L(index_t l, index_t bi);
    void update_K(index_t l, index_t bi);

    void solve_u_forward(index_t l, index_t iU, mut_view<> λ, index_t stride) const;
    void solve_y_forward(index_t l, index_t iY, mut_view<> λ, mut_view<> w, index_t stride) const;
    void solve_λ_forward(index_t l, index_t biL, mut_view<> λ, view<> w, index_t stride) const;

    /// @}

    /// @name Low-level PCR factorization and solve routines
    /// @{

    static constexpr bool merge_last_level_pcr = true;
    void factor_pcr();
    template <index_t Level>
    void factor_pcr_level();
    void factor_pcr_parallel(Context &ctx);
    template <index_t Level>
    void factor_pcr_level_parallel(Context &ctx);

    template <index_t Level>
    void solve_pcr_level(mut_batch_view<> λ, mut_batch_view<> work_pcr) const;
    void solve_pcr(mut_batch_view<> λ, mut_batch_view<> work_pcr) const;
    void solve_pcr(mut_batch_view<> λ) { solve_pcr(λ, work_pcg.batch(0).left_cols(1)); }

    /// @}

    /// @name Low-level PCG routines
    /// @{

    value_type mul_Mv(batch_view<> p, mut_batch_view<> Mp, batch_view<default_order> L,
                      batch_view<default_order> K) const;
    value_type mul_precond(batch_view<> r, mut_batch_view<> z, mut_batch_view<> w,
                           batch_view<default_order> L, batch_view<default_order> K) const;
    void solve_pcg(mut_batch_view<> λ, mut_batch_view<> work_pcg) const;
    void solve_pcg(mut_batch_view<> λ) { solve_pcg(λ, work_pcg.batch(0)); }

    /// @}

    /// @name Low-level reverse solve routines
    /// @{

    void solve_reverse_cr(Context &ctx, mut_view<> λ, mut_view<> work, index_t stride) const;
    void solve_reverse_cr_parallel(Context &ctx, mut_view<> λ, mut_view<> work,
                                   index_t stride) const;
    void solve_reverse_cr_serial(mut_view<> λ, mut_view<> work, index_t stride) const;
    void solve_u_backward(index_t l, index_t iU, mut_view<> λ, mut_view<> w, index_t stride) const;
    void solve_y_backward(index_t l, index_t iY, mut_view<> λ, index_t stride) const;
    void solve_λ_backward(index_t biL, mut_view<> λ, view<> w, index_t stride) const;

    /// @}

    /// @name Low-level factorization update routines
    /// @{

    /// @todo properly define semantics and indices.
    void set_thread_update_rank(Context &ctx, index_t c, index_t m);
    /// @todo properly define semantics and indices.
    void set_update_rank_extra(index_t m);
    /// @todo properly define semantics and indices.
    void clear_update_rank_extra();

    [[nodiscard]] std::pair<index_t, index_t> cols_Ups_fwd(index_t l, index_t i) const;
    [[nodiscard]] std::pair<index_t, index_t> cols_Ups_bwd(index_t l, index_t i) const;
    [[nodiscard]] std::pair<index_t, index_t> cols_Q_cr(index_t l, index_t i) const;
    [[nodiscard]] index_t work_Ups_fwd_w(index_t l, index_t i) const;
    [[nodiscard]] index_t work_Ups_bwd_w(index_t l, index_t i) const;
    [[nodiscard]] mut_batch_view<column_major> work_Ups_fwd(index_t l, index_t i);
    [[nodiscard]] mut_batch_view<column_major> work_Ups_bwd(index_t l, index_t i);
    [[nodiscard]] mut_batch_view<column_major> work_Q_cr(index_t l, index_t i);
    [[nodiscard]] mut_batch_view<column_major> work_Σ_fwd(index_t l, index_t i);
    [[nodiscard]] mut_batch_view<column_major> work_Σ_bwd(index_t l, index_t i);
    [[nodiscard]] mut_batch_view<column_major> work_Σ_Q(index_t l, index_t i);
    [[nodiscard]] mut_batch_view<column_major> work_Ups_fwd_last();
    [[nodiscard]] mut_batch_view<column_major> work_Ups_bwd_last();
    [[nodiscard]] mut_batch_view<column_major> work_Σ_fwd_last();
    [[nodiscard]] mut_batch_view<column_major> work_Σ_bwd_last();
    [[nodiscard]] mut_batch_view<column_major> work_Ups_extra();
    [[nodiscard]] mut_batch_view<column_major> work_Σ_extra();

    void update_cr(Context &ctx);
    void update_L(index_t l, index_t i);
    void update_U(index_t l, index_t i);
    void update_Y(index_t l, index_t i);

    template <index_t Level>
    void update_pcr_level(index_t m, mut_batch_view<> WYU, mut_batch_view<> WΣ);
    void update_pcr(batch_view<> fwd, batch_view<> bwd, batch_view<> Σ);

    /// @}

    /// @name Prefetching
    /// @{

    template <StorageOrder O>
    void prefetch(batch_view<O> X) const;
    template <StorageOrder O>
    void prefetch_L(batch_view<O> X) const;
    void prefetch_L(index_t bi) const;
    void prefetch_U(index_t l, index_t iU) const;
    void prefetch_Y(index_t l, index_t iY) const;

    /// @}
};

/// Linear solver for systems with optimal control structure.
/// @tparam VL              Vector length.
/// @tparam T               Scalar type.
/// @tparam DefaultOrder    Storage order for the matrix workspaces (row/column major).
/// @ingroup topic-ocp-solvers
template <index_t VL = 4, class T = real_t, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct CyqloneSolver {
    using value_type = T;

    /// @name Problem dimensions
    /// @{

    const index_t N_horiz; ///< Horizon length of the optimal control problem.
    const index_t nx;      ///< Number of states of the OCP.
    const index_t nu;      ///< Number of controls of the OCP.
    const index_t ny;      ///< Number of general constraints of the OCP per stage.
    const index_t ny_0;    ///< Number of general constraints at stage 0, D(0) u(0).
    const index_t ny_N;    ///< Number of general constraints at the final stage, C(N) x(N).

    /// Get the total number of primal variables in the OCP.
    /// @note The actual number of variable stored in Cyqlone's internal data structures may be
    ///       larger.
    [[nodiscard]] index_t num_variables() const { return N_horiz * (nu + nx); }
    /// Get the total number of dynamics constraints in the OCP.
    /// @note The actual number of constraints stored in Cyqlone's internal data structures may be
    ///       larger.
    [[nodiscard]] index_t num_dynamics_constraints() const { return N_horiz * nx; }
    /// Get the total number of general constraints in the OCP.
    /// @note The actual number of constraints stored in Cyqlone's internal data structures may be
    ///       larger.
    [[nodiscard]] index_t num_general_constraints() const {
        return (N_horiz - 1) * ny + ny_0 + ny_N;
    }

    /// @}

    /// @name Parallelization and vectorization
    /// @{

    /// Tricyqle solver type for solving block-tridiagonal systems in parallel.
    using tricyqle_t = TricyqleSolver<VL, T, DefaultOrder>;
    using Context    = tricyqle_t::Context;
    using simd       = typename tricyqle_t::simd;

    /// Number of processors/threads.
    const index_t p;
    /// Vector length.
    static constexpr index_t v = VL;
    /// Number of stages per thread per vector lane (rounded up).
    const index_t n = (N_horiz + p * v - 1) / (p * v);

    /// log₂(v), logarithm of the vector length.
    [[nodiscard]] constexpr index_t lv() const { return tricyqle.lv(); }
    /// log₂(p), logarithm of the number of processors/threads, rounded up.
    [[nodiscard]] constexpr index_t lp() const { return tricyqle.lp(); }
    /// The number of processors @ref p rounded up to the next power of two.
    [[nodiscard]] constexpr index_t ceil_p() const { return tricyqle.ceil_p(); }
    /// The number of parallel execution units P rounded up to the next power of two.
    [[nodiscard]] constexpr index_t ceil_P() const { return tricyqle.ceil_P(); }

    /// Run a function in parallel.
    void run(auto &&func) const { return tricyqle.run(std::forward<decltype(func)>(func)); }

    /// Call a function for each stage in the horizon, passing the stage index, the data batch
    /// index, and optionally the corresponding batches of the given arrays.
    void foreach_stage(Context &ctx, auto &&func, auto &&...xs) const {
        BATMAT_ASSERT(((xs.batch_size() == v) && ...));
        BATMAT_ASSERT(((xs.depth() == ceil_N()) && ...));
        const index_t ti = riccati_thread_assignment(ctx);
        for (index_t i = 0; i < n; ++i) {
            const index_t di = ti * n + i;
            const index_t j  = sub_wrap_N(ti * n, i);
            func(j, di, xs.batch(di)...);
        }
    }

    /// @}

    /// @name Indexing utilities
    /// @{

    /// Horizon length, rounded up to a multiple of the number of parallel execution units.
    [[nodiscard]] index_t ceil_N() const { return n * p * v; }
    /// 2-adic valuation ν₂.
    [[nodiscard]] index_t ν2(index_t i) const;
    /// 2-adic valuation modulo p, i.e. `ν2p(0) = ν2p(p) = lp()`.
    [[nodiscard]] index_t ν2p(index_t i) const;
    /// Add @p b to @p a modulo @ref N_horiz.
    [[nodiscard]] index_t add_wrap_N(index_t a, index_t b) const;
    /// Subtract @p b from @p a modulo @ref N_horiz.
    [[nodiscard]] index_t sub_wrap_N(index_t a, index_t b) const;
    /// Add @p b to @p a modulo @ref p.
    [[nodiscard]] index_t add_wrap_p(index_t a, index_t b) const;
    /// Subtract @p b from @p a modulo @ref p.
    [[nodiscard]] index_t sub_wrap_p(index_t a, index_t b) const;
    /// Add @p b to @p a modulo @ref ceil_p().
    [[nodiscard]] index_t add_wrap_ceil_p(index_t a, index_t b) const;
    /// Subtract @p b from @p a modulo @ref ceil_p().
    [[nodiscard]] index_t sub_wrap_ceil_p(index_t a, index_t b) const;

    /// @todo refactor sparse.tpp
    [[nodiscard]] index_t sub_wrap_ceil_P(index_t a, index_t b) const;
    /// @todo refactor sparse.tpp
    [[nodiscard]] index_t add_wrap_ceil_P(index_t a, index_t b) const;
    /// @todo refactor sparse.tpp
    [[nodiscard]] index_t get_linear_batch_offset(index_t biA) const;

    /// @}

    /// @name Matrix data structures
    /// @{

    /// Default storage order for most matrices.
    static constexpr auto default_order = tricyqle_t::default_order;
    /// Column-major storage order for column vectors and update matrices.
    static constexpr auto column_major = tricyqle_t::column_major;

    /// Owning type for a batch of matrices (with batch size v).
    template <StorageOrder O = column_major>
    using matrix = typename tricyqle_t::template matrix<O>;
    /// Non-owning immutable view type for @ref matrix.
    template <StorageOrder O = column_major>
    using view = typename tricyqle_t::template view<O>;
    /// Non-owning mutable view type for @ref matrix.
    template <StorageOrder O = column_major>
    using mut_view     = typename tricyqle_t::template mut_view<O>;
    using layer_stride = typename tricyqle_t::layer_stride;
    /// Non-owning immutable view type for a single batch of v matrices.
    template <StorageOrder O = column_major>
    using batch_view = typename tricyqle_t::template batch_view<O>;
    /// Non-owning mutable view type for a single batch of v matrices.
    template <StorageOrder O = column_major>
    using mut_batch_view = typename tricyqle_t::template mut_batch_view<O>;

    /// @}

    /// @name Solver parameters
    /// @{

    /// Solver parameters and settings.
    CyqloneParams<value_type> params{};

    /// Get the current Cyqlone solver parameters.
    [[nodiscard]] CyqloneParams<value_type> get_params() const { return params; }

    /// Update the Cyqlone solver parameters.
    void update_params(const CyqloneParams<value_type> &new_params) { params = new_params; }

    /// Get the current Tricyqle solver parameters.
    [[nodiscard]] TricyqleParams<value_type> get_tricyqle_params() const {
        return tricyqle.get_params();
    }

    /// Update the Tricyqle solver parameters.
    void update_tricyqle_params(const TricyqleParams<value_type> &new_params) {
        tricyqle.update_params(new_params);
    }

    /// Configure the barrier spin count used in parallel synchronization before falling back to a
    /// futex wait.
    uint32_t set_barrier_spin_count(uint32_t spin_count) {
        auto &barrier = tricyqle.parallel_ctx->barrier;
        static_assert(std::is_same_v<decltype(barrier.spin_count), decltype(spin_count)>);
        return std::exchange(barrier.spin_count, spin_count);
    }

    /// Get a string representation of the main solver parameters. Used mainly for file names.
    [[nodiscard]] std::string get_params_string() const {
        const auto &tricyqle_params = get_tricyqle_params();
        std::string_view solve      = tricyqle_params.solve_method == SolveMethod::PCR ? "pcr"
                                      : tricyqle_params.solve_method == SolveMethod::StairPCG
                                          ? "pcg=stair"
                                          : "pcg=jacobi";
        std::string_view order      = default_order == StorageOrder::RowMajor ? "rm" : "cm";
        return std::format("nx={}-nu={}-ny={}-N={}-p={}-v={}-{}-{}", nx, nu, ny, N_horiz, p, v,
                           solve, order);
    }

    /// @}

    /// @name Tricyqle solver for block-tridiagonal systems
    /// @{

    /// Block-tridiagonal solver (CR/PCR/PCG).
    tricyqle_t tricyqle = {
        .block_size = nx,
        .max_rank   = std::max(ny, ny_0 + ny_N) * N_horiz,
        .p          = p,
    };

    /// @}

    // Note: the cumbersome IILE initialization syntax is to work around a GCC bug
    //       https://gcc.gnu.org/bugzilla/show_bug.cgi?id=116015

    /// @name OCP data (reordered for use during the Cyqlone algorithm)
    /// @{

    /// Stage-wise Hessian blocks H(j) = [ R(j)  S(j);  S(j)ᵀ  Q(j) ] of the OCP cost function.
    matrix<default_order> data_H = [this] {
        return matrix<default_order>{{.depth = ceil_N(), .rows = nu + nx, .cols = nu + nx}};
    }();
    /// Stage-wise dynamics matrices F(j) = [ B(j)  A(j) ] of the OCP.
    matrix<default_order> data_F = [this] {
        return matrix<default_order>{{.depth = ceil_N(), .rows = nx, .cols = nu + nx}};
    }();
    /// Stage-wise constraint Jacobians G(j)ᵀ = [ D(j)  C(j) ]ᵀ of the OCP.
    matrix<default_order> data_Gᵀ = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<default_order>{{.depth = ceil_N(), .rows = nu + nx, .cols = nyM}};
    }();

    /// @}

    /// @name Modified Riccati data structures
    /// @{

    /// Cholesky factors of the Hessian blocks for the Riccati recursion.
    /// LH(j) = [ LR(j)  0;  LS(j)  LQ(j) ]
    matrix<default_order> riccati_LH = [this] {
        return matrix<default_order>{{.depth = p * v, .rows = nu + nx, .cols = n * (nu + nx)}};
    }();
    /// Storage for the matrices LB(j), Acl(j) and LA(j₁) for the Riccati recursion.
    /// Grouped per thread, with layout [ Acl(jₙ) ... Acl(j₂) LA(j₁) | LB(jₙ) ... LB(j₁) ], so that
    /// LA(j₁) and LB(j) are contiguous (useful when evaluating the Schur complement).
    matrix<default_order> riccati_LAB = [this] {
        return matrix<default_order>{{.depth = p * v, .rows = nx, .cols = n * nx + n * nu}};
    }();
    /// Temporary storage for the V(j) = [ B(j)ᵀ LQ(j);  A(j)ᵀ LQ(j) ] matrices during the Riccati
    /// recursion. The workspace is wider than just V to also accommodate the active constraint
    /// Jacobians, since both are used to update the Hessian blocks during the Riccati recursion.
    matrix<default_order> riccati_V = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<default_order>{{.depth = p * v, .rows = nu + nx, .cols = nx + nyM}};
    }();
    /// Temporary workspace for the Riccati solve phase.
    matrix<column_major> riccati_work = [this] {
        return matrix<column_major>{{.depth = p * v, .rows = nx, .cols = 1}};
    }();

    /// @name Riccati factorization update data structures
    /// @{

    /// Compressed representation of the nonzero diagonal elements of the matrix Σ, populated
    /// for each thread separately during the factorization update of the Riccati recursion.
    matrix<column_major> work_Σ = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = p * v, .rows = n * nyM, .cols = 1}};
    }();
    /// Workspace to store the update matrices Υu, Υx, Υλ, Φu, Φx and Φλ during the factorization
    /// update of the Riccati recursion.
    /// Both @ref riccati_Υ1 and @ref riccati_Υ2 are used alternately.
    matrix<column_major> riccati_Υ1 = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = p * v, .rows = nu + nx + nx, .cols = n * nyM}};
    }();
    /// Alternate workspace to @ref riccati_Υ1.
    matrix<column_major> riccati_Υ2 = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = p * v, .rows = nu + nx + nx, .cols = n * nyM}};
    }();

    /// @}

    /// @name Packing and unpacking of OCP data to Cyqlone storage format
    /// @{

    /// @}

    /// @name Packing and unpacking of OCP data to Cyqlone storage format
    /// @{

    /// Initialize a Cyqlone solver for the given OCP.
    ///
    /// Note: constraints on u(0) and x(N) should be independent.
    /// ~~~
    ///               nx  nu
    /// ocp.CD(0) = [ 0 | D ] ny₀
    ///             [ 0 | 0 ] ny - ny₀
    /// ~~~
    ///
    /// Since ocp.D(0) and ocp.C(N) will be merged, the top ny₀ rows of ocp.C(N)
    /// should be zero.
    ///
    /// @todo Create documentation page about the different OCP representations and storage formats.
    static CyqloneSolver build(const CyqloneStorage<value_type> &ocp, index_t p);
    /// Update the internal data structures to reflect changes in the OCP data (without changing
    /// the problem size).
    void update_data(const CyqloneStorage<value_type> &ocp);
    /// Initialize the right-hand side vector for the dynamics constraints of the OCP, using the
    /// custom Cyqlone storage format.
    void initialize_rhs(const CyqloneStorage<value_type> &ocp, mut_view<> rhs) const;
    /// @copydoc initialize_rhs
    matrix<> initialize_rhs(const CyqloneStorage<value_type> &ocp) const;
    /// Initialize the gradient vector for the OCP cost function, using the custom Cyqlone storage
    /// format.
    void initialize_gradient(const CyqloneStorage<value_type> &ocp, mut_view<> grad) const;
    /// @copydoc initialize_gradient
    matrix<> initialize_gradient(const CyqloneStorage<value_type> &ocp) const;
    /// Initialize the lower and upper bounds for the general constraints of the OCP, using the
    /// custom Cyqlone storage format.
    void initialize_bounds(const CyqloneStorage<value_type> &ocp, mut_view<> b_min,
                           mut_view<> b_max) const;
    /// @copydoc initialize_bounds
    std::pair<matrix<>, matrix<>> initialize_bounds(const CyqloneStorage<value_type> &ocp) const;

    /// @todo check and document behavior when `N_horiz != ceil_N()`.
    void pack_variables(std::span<const value_type> ux_lin, mut_view<> ux) const;
    matrix<> pack_variables(std::span<const value_type> ux_lin) const;
    void unpack_variables(view<> ux, std::span<value_type> ux_lin) const;
    std::vector<value_type> unpack_variables(view<> ux) const;
    void pack_dynamics(std::span<const value_type> λ_lin, mut_view<> λ) const;
    matrix<> pack_dynamics(std::span<const value_type> λ_lin) const;
    void unpack_dynamics(view<> λ, std::span<value_type> λ_lin) const;
    std::vector<value_type> unpack_dynamics(view<> λ) const;
    void pack_constraints(std::span<const value_type> y_lin, mut_view<> y,
                          value_type fill = 0) const;
    matrix<> pack_constraints(std::span<const value_type> y_lin, value_type fill = 0) const;
    void unpack_constraints(view<> y, std::span<value_type> y_lin) const;
    std::vector<value_type> unpack_constraints(view<> y) const;

    /// Get a zero-initialized matrix for the primal variables u and x.
    matrix<> initialize_variables() const;
    /// Get a zero-initialized matrix for the dynamics constraints (or their multipliers).
    matrix<> initialize_dynamics_constraints() const;
    /// Get a zero-initialized matrix for the general constraints (or their multipliers).
    matrix<> initialize_general_constraints() const;

    /// @}

    /// @name OCP cost gradient and constraints evaluation
    /// @{

    /// Compute Mx + b, where M is the dynamics constraint Jacobian matrix of the OCP.
    /// In other words, evaluate the residuals for all stages, i.e.,
    /// @f$ A_j x^j + B_j u^j - x^{j+1} + b^j @f$.
    void residual_dynamics_constr(Context &ctx, view<> x, view<> b, mut_view<> Mxb) const;
    /// Compute Mᵀλ, where M is the dynamics constraint Jacobian matrix of the OCP.
    /// Optionally add the result to the existing contents of Mᵀλ by setting @p accum to true.
    void transposed_dynamics_constr(Context &ctx, view<> λ, mut_view<> Mᵀλ,
                                    bool accum = false) const;
    /// Compute the general constraints Gx, where G is the general constraint Jacobian matrix of the
    /// OCP. In other words, evaluate the constraints for all stages, i.e.,
    /// @f$ C_j x^j + D_j u^j @f$.
    void general_constr(Context &ctx, view<> ux, mut_view<> DCux) const;
    /// Compute Gᵀy, where G is the general constraint Jacobian matrix of the OCP.
    void transposed_general_constr(Context &ctx, view<> y, mut_view<> DCᵀy) const;
    /// @copydoc transposed_general_constr
    /// @todo Remove.
    void transposed_general_constr(view<> y, mut_view<> DCᵀy) const;
    /// Compute the cost gradient, with optional scaling factors.
    /// grad_f ← Q ux + α q + β grad_f
    void cost_gradient(Context &ctx, view<> ux, value_type α, view<> q, value_type β,
                       mut_view<> grad_f) const;
    /// Compute the regularized cost gradient, with regularization parameter γ⁻¹, with respect to
    /// the point @p ux0.
    void cost_gradient_regularized(Context &ctx, value_type γ, view<> ux, view<> ux0, view<> q,
                                   mut_view<> grad_f) const;
    /// Subtract the regularization term from the cost gradient.
    void cost_gradient_remove_regularization(Context &ctx, value_type γ, view<> x, view<> x0,
                                             mut_view<> grad_f) const;

    /// @}

    /// @name Factorization and solve routines
    /// @{

    void factor_solve(Context &ctx, value_type γ, view<> Σ, mut_view<> ux, mut_view<> λ);
    void factor(Context &ctx, value_type γ, view<> Σ);
    void solve_forward(Context &ctx, mut_view<> ux, mut_view<> λ);
    void solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ);
    /// Perform factorization updates of the Cyqlone factorization as described by
    /// Algorithm 4 in the paper.
    void update(Context &ctx, view<> ΔΣ);

    /// @}

    /// @name Low-level factorization and forward solve routines
    /// @{

    index_t riccati_thread_assignment(Context &ctx) const { return add_wrap_p(ctx.index, 1); }
    template <bool Factor = true, bool Solve = true>
    void factor_riccati_solve(Context &ctx, value_type γ, view<> Σ, mut_view<> ux, mut_view<> λ);
    template <bool Factor = true, bool Solve = true>
    void compute_schur(Context &ctx, mut_view<> ux, mut_view<> λ);

    /// @}

    /// @name Low-level factorization and forward solve routines for parallel cyclic reduction
    /// @{

    template <bool Factor = true, bool Solve = true>
    void factor_solve_impl(Context &ctx, value_type γ, view<> Σ, mut_view<> ux, mut_view<> λ);

    /// @}

    /// @name Low-level reverse solve routines
    /// @{

    void solve_riccati_reverse(Context &ctx, mut_view<> ux, mut_view<> λ, mut_view<> work) const;
    void solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ, mut_view<> work) const;

    /// @}

    /// @name Low-level factorization update routines
    /// @{

    /// Update the modified Riccati factorization of a single block column as described by
    /// Algorithm 3 in the paper.
    void update_riccati(Context &ctx, view<> Σ);

    /// @}

    /// @name Build sparse representations for debugging and testing
    /// @{

    [[nodiscard]] SparseMatrix build_sparse(const CyqloneStorage<value_type> &ocp,
                                            std::span<const value_type> Σ) const;
    [[nodiscard]] std::vector<value_type> build_rhs(view<> ux, view<> λ) const;
    [[nodiscard]] SparseMatrix build_sparse_factor() const;
    [[nodiscard]] SparseMatrix build_sparse_diag() const;

    /// @}
};

} // namespace CYQLONE_NS(cyqlone)
