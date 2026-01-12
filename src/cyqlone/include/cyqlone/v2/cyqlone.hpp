#pragma once

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

#include "../compact.hpp" // TODO

#include <algorithm>
#include <bit>
#include <cassert>
#include <limits>
#include <utility>

namespace CYQLONE_NS(cyqlone)::v2 {

using batmat::matrix::StorageOrder;

[[nodiscard]] constexpr bool is_pow_2(index_t n) {
    BATMAT_ASSUME(n > 0);
    auto un = static_cast<std::make_unsigned_t<index_t>>(n);
    return std::has_single_bit(un);
}

[[nodiscard]] constexpr index_t get_depth(index_t n) {
    BATMAT_ASSUME(n > 0);
    auto un = static_cast<std::make_unsigned_t<index_t>>(n);
    return static_cast<index_t>(std::bit_width(un - 1));
}

[[nodiscard]] constexpr index_t get_level(index_t i) {
    BATMAT_ASSUME(i > 0);
    auto ui = static_cast<std::make_unsigned_t<index_t>>(i);
    return static_cast<index_t>(std::countr_zero(ui));
}

[[nodiscard]] constexpr index_t get_index_in_level(index_t i) {
    if (i == 0)
        return 0;
    auto l = get_level(i);
    return i >> (l + 1);
}

template <index_t VL = 4, class T = real_t, StorageOrder DefaultOrder = StorageOrder::ColMajor>
struct CyqloneSolver {
    using value_type             = T;
    using vl_t                   = std::integral_constant<index_t, VL>;
    using align_t                = std::integral_constant<index_t, VL * alignof(T)>;
    static constexpr index_t vl  = VL;
    static constexpr index_t lvl = get_depth(vl);

    const index_t N_horiz;
    const index_t nx, nu, ny, ny_0, ny_N;
    /// Number of processors/threads
    const index_t p = 8;
    [[nodiscard]] constexpr index_t lp() const { return get_depth(p); }
    [[nodiscard]] static constexpr index_t lv() { return lvl; }

    /// log2(P), logarithm of the number of parallel execution units
    /// (number of processors × vector length)
    const index_t lP = lp() + lv();
    /// Number of stages per thread per lane (rounded up)
    const index_t n = (N_horiz + p * vl - 1) / (p * vl);

    [[nodiscard]] index_t ceil_N() const { return n * p * vl; }
    [[nodiscard]] index_t ν2p(index_t bi) const;
    [[nodiscard]] index_t add_wrap_N(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_N(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_p(index_t a, index_t b) const;
    [[nodiscard]] index_t add_wrap_p(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_ceil_p(index_t a, index_t b) const;
    [[nodiscard]] index_t add_wrap_ceil_p(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_P(index_t a, index_t b) const;
    [[nodiscard]] index_t get_linear_batch_offset(index_t biA) const;

    static constexpr auto default_order = DefaultOrder;
    static constexpr auto column_major  = StorageOrder::ColMajor;

    template <StorageOrder O = column_major>
    using matrix = batmat::matrix::Matrix<value_type, index_t, vl_t, index_t, O, align_t>;
    template <StorageOrder O = column_major>
    using view = batmat::matrix::View<const value_type, index_t, vl_t, index_t, index_t, O>;
    template <StorageOrder O = column_major>
    using mut_view     = batmat::matrix::View<value_type, index_t, vl_t, index_t, index_t, O>;
    using layer_stride = batmat::matrix::DefaultStride;
    template <StorageOrder O = column_major>
    using batch_view = batmat::matrix::View<const value_type, index_t, vl_t, vl_t, layer_stride, O>;
    template <StorageOrder O = column_major>
    using mut_batch_view = batmat::matrix::View<value_type, index_t, vl_t, vl_t, layer_stride, O>;

    // TODO: these are remnant classes that are used for simple elementwise operations only, and
    //       they should be replaced by free functions similar to the ones in batmat::linalg.
    using compact_blas =
        cyqlone::compact::CompactBLAS<T, batmat::datapar::deduced_abi<T, VL>, column_major>;
    using compact_blas_default =
        cyqlone::compact::CompactBLAS<T, batmat::datapar::deduced_abi<T, VL>, default_order>;

    bool enable_prefetching        = true;
    index_t pcg_max_iter           = 100;
    value_type pcg_tolerance       = std::numeric_limits<value_type>::epsilon() / 10;
    bool pcg_print_resid           = false;
    SolveMethod solve_method       = SolveMethod::StairPCG;
    double pcr_max_update_fraction = 0.6;
    double cr_max_update_fraction  = 0.9;

    [[nodiscard]] std::string get_params_string() const {
        std::string_view solve = solve_method == SolveMethod::PCR        ? "pcr"
                                 : solve_method == SolveMethod::StairPCG ? "pcg=stair"
                                                                         : "pcg=jacobi";
        std::string_view order = default_order == StorageOrder::RowMajor ? "rm" : "cm";
        return std::format("nx={}-nu={}-ny={}-N={}-p={}-v={}-{}-{}", nx, nu, ny, N_horiz, p, VL,
                           solve, order);
    }

    using SharedContext                         = parallel::SharedContext;
    using Context                               = parallel::Context<SharedContext>;
    std::unique_ptr<SharedContext> parallel_ctx = std::make_unique<SharedContext>(p);

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
        return matrix<default_order>{{.depth = p * vl, .rows = nu + nx, .cols = n * (nu + nx)}};
    }();
    /// Storage for the matrices LB(j), Acl(j) and LA(j₁) for the Riccati recursion.
    /// Grouped per thread, with layout [ Acl(jₙ) ... Acl(j₂) LA(j₁) | LB(jₙ) ... LB(j₁) ], so that
    /// LA(j₁) and LB(j) are contiguous (useful when evaluating the Schur complement).
    matrix<default_order> riccati_LAB = [this] {
        return matrix<default_order>{{.depth = p * vl, .rows = nx, .cols = n * nx + n * nu}};
    }();
    /// Temporary storage for the V(j) = [ B(j)ᵀ LQ(j);  A(j)ᵀ LQ(j) ] matrices during the Riccati
    /// recursion. The workspace is wider than just V to also accommodate the active constraint
    /// Jacobians, since both are used to update the Hessian blocks during the Riccati recursion.
    matrix<default_order> riccati_V = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<default_order>{{.depth = p * vl, .rows = nu + nx, .cols = nx + nyM}};
    }();
    /// Temporary workspace for the Riccati solve phase.
    matrix<column_major> riccati_work = [this] {
        return matrix<column_major>{{.depth = p * vl, .rows = nx, .cols = 1}};
    }();

    /// @}

    /// @name Cyclic reduction data structures
    /// @{

    /// Diagonal blocks of the Cholesky factor of the Schur complement (used during CR).
    /// These matrices are used to evaluate the diagonal blocks M themselves first, and are then
    /// factorized in-place. After the (batched) CR phase, cr_L(0) still contains M(0), and L(0)
    /// is stored in pcr_L(0).
    matrix<default_order> cr_L = [this] {
        return matrix<default_order>{{.depth = p * vl, .rows = nx, .cols = nx}};
    }();
    /// Subdiagonal blocks U of the Cholesky factor of the Schur complement (used during CR).
    /// These matrices are associated with coupling backward in time (K˂).
    matrix<default_order> cr_U = [this] {
        return matrix<default_order>{{.depth = p * vl, .rows = nx, .cols = nx}};
    }();
    /// Subdiagonal blocks Y of the Cholesky factor of the Schur complement (used during CR).
    /// These matrices are associated with coupling forward in time (K˃).
    matrix<default_order> cr_Y = [this] {
        return matrix<default_order>{{.depth = p * vl, .rows = nx, .cols = nx}};
    }();
    /// Temporary workspace for the CR solve phase to enable parallel evaluation of matrix-vector
    /// products with U and Y without data races.
    matrix<column_major> work_cr = [this] {
        return matrix<column_major>{{.depth = p * vl, .rows = nx, .cols = 1}};
    }();

    /// @}

    /// @name Parallel cyclic reduction data structures
    /// @{

    /// Diagonal blocks of the PCR Cholesky factorizations of the block-tridiagonal system with
    /// diagonal blocks cr_L(0) and subdiagonal blocks cr_Y(0). Note that pcr_L(0) should be
    /// initialized with the Cholesky factors of cr_L(0) before performing PCR.
    matrix<default_order> pcr_L = [this] {
        return matrix<default_order>{{.depth = VL * (lvl + 1), .rows = nx, .cols = nx}};
    }();
    /// Subdiagonal blocks Y of the PCR Cholesky factorizations.
    matrix<default_order> pcr_Y = [this] {
        return matrix<default_order>{{.depth = VL * lvl, .rows = nx, .cols = nx}};
    }();
    /// Subdiagonal blocks U of the PCR Cholesky factorizations.
    matrix<default_order> pcr_U = [this] {
        return matrix<default_order>{{.depth = VL * lvl, .rows = nx, .cols = nx}};
    }();
    /// Workspace to store the diagonal blocks during the PCR factorization.
    matrix<default_order> pcr_M = [this] {
        return matrix<default_order>{{.depth = VL, .rows = nx, .cols = nx}};
    }();
    /// Temporary workspace for CG vectors.
    matrix<column_major> work_pcg = [this] {
        return matrix<column_major>{{.depth = vl, .rows = nx, .cols = 4}};
    }();

    /// @}

    /// @name Factorization update data structures
    /// @{

    /// Update rank (number of changing constraints) per thread. Replaced by their partial sums
    /// over all threads before the update of the Schur complement.
    std::vector<index_t> nJs = std::vector<index_t>(p);
    /// Compressed representation of the nonzero diagonal elements of the matrix Σ, populated
    /// for each thread separately during the factorization update of the Riccati recursion, and
    /// later compressed across all threads into @ref work_update_Σ so it can be applied to the
    /// Schur complement.
    matrix<column_major> work_Σ = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = p * vl, .rows = n * nyM, .cols = 1}};
    }();
    /// Workspace to store the update matrices Υu, Υx, Υλ, Φu, Φx and Φλ during the factorization
    /// update of the Riccati recursion.
    /// Both @ref riccati_Υ1 and @ref riccati_Υ2 are used alternately.
    matrix<column_major> riccati_Υ1 = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = p * vl, .rows = nu + nx + nx, .cols = n * nyM}};
    }();
    /// Alternate workspace to @ref riccati_Υ1.
    matrix<column_major> riccati_Υ2 = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = p * vl, .rows = nu + nx + nx, .cols = n * nyM}};
    }();

    /// Compressed reprentation of the nonzero diagonal elements of the matrix Σ, with their indices
    /// matching the column indices of the update matrices in @ref work_update. Used during the
    /// factorization update of the Schur complement. Initialized by the values in @ref work_Σ.
    /// @todo Consider reusing @ref work_Σ directly.
    matrix<column_major> work_update_Σ = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = 1 << lvl, .rows = n * p * nyM, .cols = 1}};
    }();
    /// Workspace to store the update matrices Ξ(Υ) for the factorization update of the Schur
    /// complement. They get wider at higher levels of the CR tree, because more stages are merged.
    /// We use a clever trick to minimize data movement between levels: at each level, we perform
    /// the same operation that introduces zeros in the top two blocks of the update matrix, and
    /// introduces non-zeros in two blocks in the bottom part (see §6.4 “Cyclic reduction
    /// factorization updates of the Schur complement”).
    /// At each level l, work_update[l % 4] contains the part of the update matrix that is being
    /// zeroed out while updating L. After the update, work_update[l % 4] contains the hyperbolic
    /// Householder reflectors that are then applied to the blocks U and Y below it: depending on
    /// the index of the block, the matrices that update U and Y in the current level are either at
    /// work_update[(l + 1) % 4] or work_update[(l + 2) % 4]. The results of the hyperbolic
    /// Householder transformations applied to these update matrices are either stored in the same
    /// workspace, or at work_update[(l + 3) % 4]. This is done in a way such that all update
    /// matrices for L in the next level all end up at work_update[(l+1) % 4], and in a way that
    /// leaves room for the nonzeros that are introduced in the update matrices for U and Y in the
    /// next level. Since only four workspaces are ever used concurrently, we can cycle through them
    /// cyclically, hence the modulo 4 in the indexing above.
    /// The update matrices do not move "horizontally" in memory, the column index for each rank-1
    /// update is computed based on the values in @ref nJs at the beginning of the procedure,
    /// ensuring that update matrices applied to L are contiguous, even though they consist of the
    /// concatenation of two update matrices from the previous level.
    matrix<column_major> work_update = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = 4 << lvl, .rows = nx, .cols = n * p * nyM}};
    }();
    /// Storage for the hyperbolic Householder transformations during the factorization update of
    /// the Schur complement. Together with the reflector vectors stored in @ref work_update, these
    /// form the matrices Q̆ that are applied to the subdiagonal blocks U, and Y.
    /// The dimensions depend on the block size used by the linear algebra, and is architecture
    /// dependent.
    /// @todo We may only need half as many (p * vl / 2).
    matrix<column_major> work_hyh = [this] {
        using namespace batmat::linalg;
        const auto [r, c] = hyhound_size_W(tril(cr_L.batch(0)));
        return matrix<column_major>{{.depth = p * vl, .rows = r, .cols = c}};
    }();

    /// Two copies of @ref work_update_Σ, with different rotations for use during the factorization
    /// updates of the PCR factorization of the last block of the Schur complement.
    /// @todo Reuse @ref work_update_Σ?
    matrix<column_major> work_update_pcr_Σ = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = VL, .rows = 2 * N_horiz * nyM, .cols = 1}};
    }();
    /// Update matrices to apply to the diagonal blocks L during the factorization update of the PCR
    /// factorization of the last block of the Schur complement.
    /// @todo Merge with @ref work_update?
    matrix<column_major> work_update_pcr_L = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = VL, .rows = nx, .cols = N_horiz * nyM}};
    }();
    /// Update matrices to apply to the subdiagonal blocks U and Y during the factorization update
    /// of the PCR factorization of the last block of the Schur complement.
    /// @todo Merge with @ref work_update?
    matrix<column_major> work_update_pcr_UY = [this] {
        const auto nyM = std::max(ny, ny_0 + ny_N);
        return matrix<column_major>{{.depth = VL, .rows = nx, .cols = 2 * N_horiz * nyM}};
    }();

    /// @}

    /// Constraints on u(0) and x(N) should be independent.
    ///
    ///                  nx  nu
    ///    ocp.CD(0) = [ 0 | D ] ny₀
    ///                [ 0 | 0 ] ny - ny₀
    ///
    /// Since ocp.D(0) and ocp.C(N) will be merged, the top ny₀ rows of ocp.C(N)
    /// should be zero.
    static CyqloneSolver build(const CyqloneStorage<value_type> &ocp, index_t p);
    void update_data(const CyqloneStorage<value_type> &ocp);
    void initialize_rhs(const CyqloneStorage<value_type> &ocp, mut_view<> rhs) const;
    matrix<> initialize_rhs(const CyqloneStorage<value_type> &ocp) const {
        matrix<> rhs = initialize_dynamics_constraints();
        initialize_rhs(ocp, rhs);
        return rhs;
    }
    void initialize_gradient(const CyqloneStorage<value_type> &ocp, mut_view<> grad) const;
    matrix<> initialize_gradient(const CyqloneStorage<value_type> &ocp) const {
        matrix<> grad = initialize_variables();
        initialize_gradient(ocp, grad);
        return grad;
    }
    void initialize_bounds(const CyqloneStorage<value_type> &ocp, mut_view<> b_min,
                           mut_view<> b_max) const;
    std::pair<matrix<>, matrix<>> initialize_bounds(const CyqloneStorage<value_type> &ocp) const {
        std::pair b{initialize_general_constraints(), initialize_general_constraints()};
        initialize_bounds(ocp, b.first, b.second);
        return b;
    }
    void pack_variables(std::span<const value_type> ux_lin, mut_view<> ux) const;
    matrix<> pack_variables(std::span<const value_type> ux_lin) const {
        matrix<> ux = initialize_variables();
        pack_variables(ux_lin, ux);
        return ux;
    }
    void unpack_variables(view<> ux, std::span<value_type> ux_lin) const;
    std::vector<value_type> unpack_variables(view<> ux) const {
        std::vector<value_type> ux_lin(num_variables());
        unpack_variables(ux, ux_lin);
        return ux_lin;
    }
    void pack_dynamics(std::span<const value_type> λ_lin, mut_view<> λ) const;
    matrix<> pack_dynamics(std::span<const value_type> λ_lin) const {
        matrix<> λ = initialize_dynamics_constraints();
        pack_dynamics(λ_lin, λ);
        return λ;
    }
    void unpack_dynamics(view<> λ, std::span<value_type> λ_lin) const;
    std::vector<value_type> unpack_dynamics(view<> λ) const {
        std::vector<value_type> λ_lin(num_dynamics_constraints());
        unpack_dynamics(λ, λ_lin);
        return λ_lin;
    }
    void pack_constraints(std::span<const value_type> y_lin, mut_view<> y,
                          value_type fill = 0) const;
    matrix<> pack_constraints(std::span<const value_type> y_lin, value_type fill = 0) const {
        matrix<> y = initialize_general_constraints();
        pack_constraints(y_lin, y, fill);
        return y;
    }
    void unpack_constraints(view<> y, std::span<value_type> y_lin) const;
    std::vector<value_type> unpack_constraints(view<> y) const {
        std::vector<value_type> y_lin(num_general_constraints());
        unpack_constraints(y, y_lin);
        return y_lin;
    }

    [[nodiscard]] index_t num_variables() const { return N_horiz * (nu + nx); }
    [[nodiscard]] index_t num_dynamics_constraints() const { return N_horiz * nx; }
    [[nodiscard]] index_t num_general_constraints() const {
        return (N_horiz - 1) * ny + ny_0 + ny_N;
    }

    matrix<> initialize_variables() const {
        return matrix<>{{.depth = ceil_N(), .rows = nu + nx, .cols = 1}};
    }
    matrix<> initialize_dynamics_constraints() const {
        return matrix<>{{.depth = ceil_N(), .rows = nx, .cols = 1}};
    }
    matrix<> initialize_general_constraints() const {
        return matrix<>{{.depth = ceil_N(), .rows = std::max(ny, ny_0 + ny_N), .cols = 1}};
    }

    void residual_dynamics_constr(Context &ctx, view<> x, view<> b, mut_view<> Mxb) const;
    void transposed_dynamics_constr(Context &ctx, view<> λ, mut_view<> Mᵀλ,
                                    bool accum = false) const;
    void general_constr(Context &ctx, view<> ux, mut_view<> DCux) const;
    void transposed_general_constr(Context &ctx, view<> y, mut_view<> DCᵀy) const;
    void transposed_general_constr(view<> y, mut_view<> DCᵀy) const;
    /// grad_f ← Q ux + a q + b grad_f
    void cost_gradient(Context &ctx, view<> ux, value_type a, view<> q, value_type b,
                       mut_view<> grad_f) const;
    void cost_gradient_regularized(Context &ctx, value_type γ, view<> ux, view<> ux0, view<> q,
                                   mut_view<> grad_f) const;
    void cost_gradient_remove_regularization(Context &ctx, value_type γ, view<> x, view<> x0,
                                             mut_view<> grad_f) const;

    template <bool Factor = true, bool Solve = true>
    void factor_riccati_solve(Context &ctx, value_type γ, view<> Σ, mut_view<> ux, mut_view<> λ);
    template <bool Factor = true, bool Solve = true>
    void compute_schur(Context &ctx, mut_view<> ux, mut_view<> λ);
    [[nodiscard]] index_t cr_thread_assignment(index_t l, index_t c) const;
    void factor_U(index_t l, index_t biU);
    void factor_Y(index_t l, index_t biY);
    void factor_L(index_t l, index_t bi);
    void update_K(index_t l, index_t bi);
    void factor_pcr();
    template <index_t Level>
    void factor_pcr_level();
    template <bool Factor = true, bool Solve = true>
    void factor_solve_impl(Context &ctx, value_type γ, view<> Σ, mut_view<> ux, mut_view<> λ);
    void factor_solve(Context &ctx, value_type γ, view<> Σ, mut_view<> ux, mut_view<> λ);
    void factor(Context &ctx, value_type γ, view<> Σ);

    void solve_u_forward(index_t l, index_t biU, mut_view<> λ) const;
    void solve_y_forward(index_t l, index_t biY, mut_view<> λ, mut_view<> w) const;
    void solve_λ_forward(index_t l, index_t biL, mut_view<> λ, view<> w) const;
    void solve_forward(Context &ctx, mut_view<> ux, mut_view<> λ);

    template <index_t Level>
    void solve_pcr_level(mut_batch_view<> λ, mut_batch_view<> work_pcr) const;
    void solve_pcr(mut_batch_view<> λ, mut_batch_view<> work_pcr) const;
    void solve_pcr(mut_batch_view<> λ) { solve_pcr(λ, work_pcg.batch(0).left_cols(1)); }

    value_type mul_Mv(batch_view<> p, mut_batch_view<> Mp, batch_view<default_order> L,
                      batch_view<default_order> K) const;
    value_type mul_precond(batch_view<> r, mut_batch_view<> z, mut_batch_view<> w,
                           batch_view<default_order> L, batch_view<default_order> K) const;
    void solve_pcg(mut_batch_view<> λ, mut_batch_view<> work_pcg) const;
    void solve_pcg(mut_batch_view<> λ) { solve_pcg(λ, work_pcg.batch(0)); }

    void solve_riccati_reverse(Context &ctx, mut_view<> ux, mut_view<> λ, mut_view<> work) const;
    void solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ);
    void solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ, mut_view<> work) const;
    void solve_u_backward(index_t l, index_t biU, mut_view<> λ, mut_view<> w) const;
    void solve_y_backward(index_t l, index_t biY, mut_view<> λ) const;
    void solve_λ_backward(index_t biL, mut_view<> λ, view<> w) const;
    template <StorageOrder O>
    void prefetch(batch_view<O> X) const {
        if (!enable_prefetching)
            return;
        const auto inner_stride = std::max<index_t>(64 / sizeof(value_type) / vl, 1);
        if constexpr (O == StorageOrder::RowMajor)
            for (index_t r = 0; r < X.rows(); ++r)
                BATMAT_UNROLLED_IVDEP_FOR (8, index_t c = 0; c < X.cols(); c += inner_stride)
                    __builtin_prefetch(&X(0, r, c), 0, 2);
        else
            for (index_t c = 0; c < X.cols(); ++c)
                BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = 0; r < X.rows(); r += inner_stride)
                    __builtin_prefetch(&X(0, r, c), 0, 2);
    }
    template <StorageOrder O>
    void prefetch_L(batch_view<O> X) const {
        if (!enable_prefetching)
            return;
        const auto inner_stride = std::max<index_t>(64 / sizeof(value_type) / vl, 1);
        if constexpr (O == StorageOrder::RowMajor)
            for (index_t r = 0; r < X.rows(); ++r)
                BATMAT_UNROLLED_IVDEP_FOR (8, index_t c = 0; c <= r; c += inner_stride)
                    __builtin_prefetch(&X(0, r, c), 0, 2);
        else
            for (index_t c = 0; c < X.cols(); ++c)
                BATMAT_UNROLLED_IVDEP_FOR (8, index_t r = c; r < X.rows(); r += inner_stride)
                    __builtin_prefetch(&X(0, r, c), 0, 2);
    }
    void prefetch_L(index_t bi) const {
        GUANAQO_TRACE("prefetch L", bi);
        prefetch_L(cr_L.batch(bi));
    }
    void prefetch_U(index_t biU) const {
        GUANAQO_TRACE("prefetch U", biU);
        prefetch(cr_U.batch(biU));
    }
    void prefetch_Y(index_t biY) const {
        GUANAQO_TRACE("prefetch Y", biY);
        prefetch(cr_Y.batch(biY));
    }

    void update_riccati(Context &ctx, view<> Σ);
    void update_L(index_t l, index_t bi);
    void update_U(index_t l, index_t bi);
    void update_Y(index_t l, index_t bi);
    void update(Context &ctx, view<> ΔΣ);

    template <index_t Level>
    void update_pcr_level(index_t m, mut_batch_view<> WYU, mut_batch_view<> WΣ);
    void update_pcr(batch_view<> fwd, batch_view<> bwd, batch_view<> Σ);

    [[nodiscard]] SparseMatrix build_sparse(const CyqloneStorage<value_type> &ocp,
                                            std::span<const value_type> Σ) const;
    [[nodiscard]] std::vector<value_type> build_rhs(view<> ux, view<> λ) const;
    [[nodiscard]] SparseMatrix build_sparse_factor() const;
    [[nodiscard]] SparseMatrix build_sparse_diag() const;
};

namespace detail {
template <class T1, class I1, class S1, guanaqo::StorageOrder O1, class T2, class I2, class S2,
          guanaqo::StorageOrder O2>
void copy(guanaqo::MatrixView<T1, I1, S1, O1> src, guanaqo::MatrixView<T2, I2, S2, O2> dst) {
    assert(src.rows == dst.rows);
    assert(src.cols == dst.cols);
    for (index_t r = 0; r < src.rows; ++r) // TODO: optimize
        for (index_t c = 0; c < src.cols; ++c)
            dst(r, c) = src(r, c);
}
template <class T0, class T1, class I1, class S1, guanaqo::StorageOrder O1, class T2, class I2,
          class S2, guanaqo::StorageOrder O2>
void scale(T0 scalar, guanaqo::MatrixView<T1, I1, S1, O1> src,
           guanaqo::MatrixView<T2, I2, S2, O2> dst) {
    assert(src.rows == dst.rows);
    assert(src.cols == dst.cols);
    for (index_t r = 0; r < src.rows; ++r) // TODO: optimize
        for (index_t c = 0; c < src.cols; ++c)
            dst(r, c) = scalar * src(r, c);
}
} // namespace detail

} // namespace CYQLONE_NS(cyqlone)::v2
