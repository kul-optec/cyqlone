#pragma once

#include <cyqlone/config.hpp>
#include <cyqlone/cyqlone-params.hpp>
#include <cyqlone/cyqlone-storage.hpp>
#include <cyqlone/parallel.hpp>
#include <cyqlone/timing.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>
#include <batmat/linalg/hyhound.hpp> // TODO: isolate size functions
#include <batmat/matrix/layout.hpp>
#include <batmat/matrix/matrix.hpp>
#include <batmat/openmp.h>
#include <batmat/simd.hpp>
#include <guanaqo/trace.hpp>

#include "compact.hpp" // TODO

#include <algorithm>
#include <bit>
#include <cassert>
#include <limits>

namespace CYQLONE_NS(cyqlone) {

using batmat::matrix::StorageOrder;

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

    /// log2(P), logarithm of the number of parallel execution units
    /// (number of processors × vector length)
    const index_t lP = lvl + 3;

    const index_t ceil_N = ((N_horiz + (1 << lP) - 1) / (1 << lP)) * (1 << lP);

    [[nodiscard]] index_t add_wrap_N(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_N(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_PmV(index_t a, index_t b) const;
    [[nodiscard]] index_t add_wrap_PmV(index_t a, index_t b) const;
    [[nodiscard]] index_t sub_wrap_P(index_t a, index_t b) const;
    [[nodiscard]] index_t get_linear_batch_offset(index_t biA) const;

    template <StorageOrder O = StorageOrder::ColMajor>
    using matrix = batmat::matrix::Matrix<value_type, index_t, vl_t, index_t, O, align_t>;
    template <StorageOrder O = StorageOrder::ColMajor>
    using mask_matrix = batmat::matrix::Matrix<bool, index_t, vl_t, index_t, O, align_t>;
    template <StorageOrder O = StorageOrder::ColMajor>
    using view = batmat::matrix::View<const value_type, index_t, vl_t, index_t, index_t, O>;
    template <StorageOrder O = StorageOrder::ColMajor>
    using mut_view     = batmat::matrix::View<value_type, index_t, vl_t, index_t, index_t, O>;
    using layer_stride = batmat::matrix::DefaultStride;
    template <StorageOrder O = StorageOrder::ColMajor>
    using batch_view = batmat::matrix::View<const value_type, index_t, vl_t, vl_t, layer_stride, O>;
    template <StorageOrder O = StorageOrder::ColMajor>
    using mut_batch_view = batmat::matrix::View<value_type, index_t, vl_t, vl_t, layer_stride, O>;

    static constexpr auto default_order = DefaultOrder;

    using compact_blas = cyqlone::compact::CompactBLAS<T, batmat::datapar::deduced_abi<T, VL>,
                                                       StorageOrder::ColMajor>; // TODO
    using compact_blas_default =
        cyqlone::compact::CompactBLAS<T, batmat::datapar::deduced_abi<T, VL>,
                                      default_order>; // TODO

    bool alt                       = true;
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
        return std::format("nx={}-nu={}-ny={}-N={}-p={}-v={}-{}{}-{}", nx, nu, ny, N_horiz,
                           1 << (lP - lvl), VL, solve, alt ? "-alt" : "", order);
    }

    using SharedContext                         = parallel::SharedContext;
    using Context                               = parallel::Context<SharedContext>;
    std::unique_ptr<SharedContext> parallel_ctx = std::make_unique<SharedContext>(1 << (lP - lvl));

    matrix<default_order> coupling_D = [this] {
        return matrix<default_order>{{
            .depth = 1 << lP,
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<default_order> coupling_U = [this] {
        return matrix<default_order>{{
            .depth = 1 << lP,
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<default_order> coupling_Y = [this] {
        return matrix<default_order>{{
            .depth = 1 << lP,
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<default_order> pcr_L = [this] {
        return matrix<default_order>{{
            .depth = VL * (lvl + 1),
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<default_order> pcr_Y = [this] {
        return matrix<default_order>{{
            .depth = VL * lvl,
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<default_order> pcr_U = [this] {
        return matrix<default_order>{{
            .depth = VL * lvl,
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<default_order> pcr_A = [this] {
        return matrix<default_order>{{
            .depth = VL,
            .rows  = nx,
            .cols  = nx,
        }};
    }();
    matrix<StorageOrder::ColMajor> work_update_pcr_L = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = VL,
            .rows  = nx,
            .cols  = ceil_N * std::max(ny, ny_0 + ny_N),
        }};
    }(); // TODO: merge with work_update?
    matrix<StorageOrder::ColMajor> work_update_pcr_UY = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = VL,
            .rows  = nx,
            .cols  = 2 * ceil_N * std::max(ny, ny_0 + ny_N),
        }};
    }(); // TODO: merge with work_update?
    matrix<StorageOrder::ColMajor> work_update_pcr_Σ = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = VL,
            .rows  = 2 * ceil_N * std::max(ny, ny_0 + ny_N),
            .cols  = 1,
        }};
    }();
    matrix<StorageOrder::ColMajor> work_update = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = 4 << lvl,
            .rows  = nx,
            .cols  = (ceil_N >> lvl) * std::max(ny, ny_0 + ny_N),
        }};
    }(); // TODO: merge with riccati_ΥΓ?
    matrix<StorageOrder::ColMajor> work_update_Σ = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = 1 << lvl,
            .rows  = (ceil_N >> lvl) * std::max(ny, ny_0 + ny_N),
            .cols  = 1,
        }};
    }();
    matrix<StorageOrder::ColMajor> work_hyh = [this] {
        using namespace batmat::linalg;
        const auto [r, c] = hyhound_size_W(tril(coupling_D.batch(0)));
        return matrix<StorageOrder::ColMajor>{{.depth = 1 << lP, .rows = r, .cols = c}};
    }();
    matrix<default_order> riccati_ÂB̂ = [this] {
        return matrix<default_order>{{
            .depth = 1 << lP,
            .rows  = nx,
            .cols  = (ceil_N >> lP) * (nu + nx),
        }};
    }();
    matrix<default_order> riccati_BAᵀ = [this] {
        return matrix<default_order>{{
            .depth = 1 << lP,
            .rows  = nu + nx,
            .cols  = ((ceil_N >> lP) - 1) * nx + std::max(ny, ny_0 + ny_N),
        }};
    }();
    matrix<default_order> riccati_R̂ŜQ̂ = [this] {
        return matrix<default_order>{{
            .depth = 1 << lP,
            .rows  = nu + nx,
            .cols  = (ceil_N >> lP) * (nu + nx),
        }};
    }();
    matrix<StorageOrder::ColMajor> riccati_ΥΓ1 = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = 1 << lP,
            .rows  = nu + nx + nx,
            .cols  = (ceil_N >> lP) * std::max(ny, ny_0 + ny_N),
        }};
    }();
    matrix<StorageOrder::ColMajor> riccati_ΥΓ2 = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = 1 << lP,
            .rows  = nu + nx + nx,
            .cols  = (ceil_N >> lP) * std::max(ny, ny_0 + ny_N),
        }};
    }();
    matrix<default_order> data_BA = [this] {
        return matrix<default_order>{{
            .depth = ceil_N,
            .rows  = nx,
            .cols  = nu + nx,
        }};
    }();
    matrix<default_order> data_DCᵀ = [this] {
        return matrix<default_order>{{
            .depth = ceil_N,
            .rows  = nu + nx,
            .cols  = std::max(ny, ny_0 + ny_N),
        }};
    }();
    matrix<StorageOrder::ColMajor> data_rhs_constr = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = ceil_N,
            .rows  = nx,
            .cols  = 1,
        }};
    }();
    matrix<StorageOrder::ColMajor> work_Σ = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = 1 << lP,
            .rows  = (ceil_N >> lP) * std::max(ny, ny_0 + ny_N),
            .cols  = 1,
        }};
    }();
    matrix<default_order> data_RSQ = [this] {
        return matrix<default_order>{{
            .depth = ceil_N,
            .rows  = nu + nx,
            .cols  = nu + nx,
        }};
    }();
    matrix<StorageOrder::ColMajor> riccati_work = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = 1 << lP,
            .rows  = nx,
            .cols  = 1,
        }};
    }();
    matrix<StorageOrder::ColMajor> work_pcg = [this] {
        return matrix<StorageOrder::ColMajor>{{
            .depth = vl,
            .rows  = nx,
            .cols  = 4,
        }};
    }();
    std::vector<index_t> nJs = std::vector<index_t>(1 << (lP - lvl));

    struct Timings {
        using type    = DefaultTimings;
        using timed_t = guanaqo::Timed<batmat::DefaultTimings>;
        type breakpoints{};
        type calc_y_hat{};
        type calc_y_hat_AT{};
        type update_active_set_change{};
        type update_factorization{};
        type factor{};
        type solve{};
        type solve_MT{};
        type solve_A{};
        type solve_grad{};
        type solve_resid{};
        type recompute_outer_grad{};
        type recompute_outer_A{};
        type recompute_outer_AT{};
        type recompute_outer_MT{};
        type recompute_outer_norm{};
        type recompute_inner_grad{};
        type recompute_inner_A{};
        type recompute_inner_MT{};
        type ineq_constr_resid{};
        type ineq_constr_viol{};
        type ineq_constr_resid_al{};
    };

    /// Constraints on u(0) and x(N) should be independent.
    ///
    ///                  nx  nu
    ///    ocp.CD(0) = [ 0 | D ] ny₀
    ///                [ 0 | 0 ] ny - ny₀
    ///
    /// Since ocp.D(0) and ocp.C(N) will be merged, the top ny₀ rows of ocp.C(N)
    /// should be zero.
    static CyqloneSolver build(const CyqloneStorage<value_type> &ocp, index_t lP);
    void update_data(const CyqloneStorage<value_type> &ocp);
    void initialize_rhs(const CyqloneStorage<value_type> &ocp, mut_view<> rhs) const;
    void initialize_gradient(const CyqloneStorage<value_type> &ocp, mut_view<> grad) const;
    void initialize_bounds(const CyqloneStorage<value_type> &ocp, mut_view<> b_min,
                           mut_view<> b_max) const;
    void pack_variables(std::span<const value_type> ux_lin, mut_view<> ux) const;
    void unpack_variables(view<> ux, std::span<value_type> ux_lin) const;
    void pack_dynamics(std::span<const value_type> λ_lin, mut_view<> λ) const;
    void unpack_dynamics(view<> λ, std::span<value_type> λ_lin) const;
    void pack_constraints(std::span<const value_type> y_lin, mut_view<> y,
                          value_type fill = 0) const;
    void unpack_constraints(view<> y, std::span<value_type> y_lin) const;

    [[nodiscard]] index_t num_variables() const { return N_horiz * (nu + nx); }
    [[nodiscard]] index_t num_dynamics_constraints() const { return N_horiz * nx; }
    [[nodiscard]] index_t num_general_constraints() const {
        return (N_horiz - 1) * ny + ny_0 + ny_N;
    }

    matrix<> initialize_variables() const {
        return matrix<>{{.depth = ceil_N, .rows = nu + nx, .cols = 1}};
    }
    matrix<> initialize_dynamics_constraints() const {
        return matrix<>{{.depth = ceil_N, .rows = nx, .cols = 1}};
    }
    matrix<> initialize_general_constraints() const {
        return matrix<>{{.depth = ceil_N, .rows = std::max(ny, ny_0 + ny_N), .cols = 1}};
    }
    mask_matrix<> initialize_active_set() const {
        return mask_matrix<>{{.depth = ceil_N, .rows = std::max(ny, ny_0 + ny_N), .cols = 1}};
    }

    // For lgp = 5, lgv = 2, N = 3 << lgp
    //
    // | Stage k | Thread t | Index i | Data di | λ(A) | λ(I) | bλ(A) | bλ(I) |
    // |:-------:|:--------:|:-------:|:-------:|-----:|-----:|------:|------:|
    // | 0/96    | 0        | 0       | 0       | 0    | 93   | 0     | 7*    |
    // | 95      | 0        | 1       | 1       |      |      |       |       |
    // | 94      | 0        | 2       | 2       |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 3       | 1        | 0       | 3       | 3    | 0    | 1     | 0     |
    // | 2       | 1        | 1       | 4       |      |      |       |       |
    // | 1       | 1        | 2       | 5       |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 6       | 2        | 0       | 6       | 6    | 3    | 2     | 1     |
    // | 5       | 2        | 1       | 7       |      |      |       |       |
    // | 4       | 2        | 2       | 8       |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 9       | 3        | 0       | 9       | 9    | 6    | 3     | 2     |
    // | 8       | 3        | 1       | 10      |      |      |       |       |
    // | 7       | 3        | 2       | 11      |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 12      | 4        | 0       | 12      | 12   | 9    | 4     | 3     |
    // | 11      | 4        | 1       | 13      |      |      |       |       |
    // | 10      | 4        | 2       | 14      |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 15      | 5        | 0       | 15      | 15   | 12   | 5     | 4     |
    // | 14      | 5        | 1       | 16      |      |      |       |       |
    // | 13      | 5        | 2       | 17      |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 18      | 6        | 0       | 18      | 18   | 15   | 6     | 5     |
    // | 17      | 6        | 1       | 19      |      |      |       |       |
    // | 16      | 6        | 2       | 20      |      |      |       |       |
    // |         |          |         |         |      |      |       |       |
    // | 21      | 7        | 0       | 21      | 21   | 18   | 7     | 6     |
    // | 20      | 7        | 1       | 22      |      |      |       |       |
    // | 19      | 7        | 2       | 23      |      |      |       |       |

    [[nodiscard]] bool is_active(index_t l, index_t bi) const;
    [[nodiscard]] index_t ν2p(index_t bi) const;
    [[nodiscard]] bool is_U_below_Y(index_t l, index_t bi) const;

    void residual_dynamics_constr(Context &ctx, view<> x, view<> b, mut_view<> Mxb) const;
    void transposed_dynamics_constr(Context &ctx, view<> λ, mut_view<> Mᵀλ) const;
    void general_constr(Context &ctx, view<> ux, mut_view<> DCux) const;
    void transposed_general_constr(Context &ctx, view<> y, mut_view<> DCᵀy) const;
    void transposed_general_constr(view<> y, mut_view<> DCᵀy) const;
    /// grad_f ← Q ux + a q + b grad_f
    void cost_gradient(Context &ctx, view<> ux, value_type a, view<> q, value_type b,
                       mut_view<> grad_f) const;
    void cost_gradient_regularized(Context &ctx, value_type S, view<> ux, view<> ux0, view<> q,
                                   mut_view<> grad_f) const;
    void cost_gradient_remove_regularization(Context &ctx, value_type S, view<> x, view<> x0,
                                             mut_view<> grad_f) const;

    void factor_U(index_t l, index_t biU);
    void factor_Y(index_t l, index_t biY);
    void factor_L(index_t l, index_t bi);
    void update_K(index_t l, index_t bi);
    void factor_pcr();
    template <index_t Level>
    void factor_pcr_level();
    void factor_l0(Context &ctx);
    void factor_riccati(Context &ctx, bool alt, value_type S, view<> Σ);
    void factor(Context &ctx, value_type S, view<> Σ, bool alt = false);

    void solve_active(index_t l, index_t biY, mut_view<> λ) const;
    void solve_active_secondary(index_t l, index_t biU, mut_view<> λ) const;
    void solve_riccati_forward(Context &ctx, mut_view<> ux, mut_view<> λ) const;
    /// Preserves b in λ (except for coupling equations solved using CR)
    void solve_riccati_forward_alt(Context &ctx, mut_view<> ux, mut_view<> λ,
                                   mut_view<> work) const;
    void solve_forward(Context &ctx, mut_view<> ux, mut_view<> λ, mut_batch_view<> work_pcg,
                       mut_view<> work) const;

    void solve_pcr(mut_batch_view<> λ, mut_batch_view<> work_pcr) const;
    void solve_pcr(mut_batch_view<> λ) { solve_pcr(λ, work_pcg.batch(0).left_cols(1)); }
    template <index_t Level>
    void solve_pcr_level(mut_batch_view<> λ, mut_batch_view<> work_pcr) const;

    value_type mul_A(batch_view<> p, mut_batch_view<> Ap, batch_view<default_order> L,
                     batch_view<default_order> B) const;
    value_type mul_precond(batch_view<> r, mut_batch_view<> z, mut_batch_view<> w,
                           batch_view<default_order> L, batch_view<default_order> B) const;
    void solve_pcg(mut_batch_view<> λ, mut_batch_view<> work_pcg) const;
    void solve_pcg(mut_batch_view<> λ) { solve_pcg(λ, work_pcg.batch(0)); }

    void solve_reverse_active(index_t l, index_t bi, mut_view<> λ) const;
    void solve_riccati_reverse(Context &ctx, mut_view<> ux, mut_view<> λ, mut_view<> work) const;
    void solve_riccati_reverse_alt(Context &ctx, mut_view<> ux, mut_view<> λ,
                                   mut_view<> work) const;
    void solve_reverse(Context &ctx, mut_view<> ux, mut_view<> λ, mut_view<> work) const;
    void solve(Context &ctx, mut_view<> ux, mut_view<> λ, mut_batch_view<> work_pcg,
               mut_view<> work_riccati) const;
    void solve(Context &ctx, mut_view<> ux, mut_view<> λ) {
        solve(ctx, ux, λ, work_pcg.batch(0), riccati_work);
    }

    void update_L(index_t l, index_t bi);
    void update_U(index_t l, index_t bi);
    void update_Y(index_t l, index_t bi);
    void update(Context &ctx, view<> ΔΣ);
    void update_riccati(Context &ctx, view<> Σ);

    void update_pcr(batch_view<> fwd, batch_view<> bwd, batch_view<> Σ);
    template <index_t Level>
    void update_pcr_level(index_t m, mut_batch_view<> WYU, mut_batch_view<> WΣ);

    std::vector<std::tuple<index_t, index_t, value_type>>
    build_sparse(const CyqloneStorage<value_type> &ocp, std::span<const value_type> Σ) const;
    std::vector<value_type> build_rhs(view<> ux, view<> λ) const;
    std::vector<std::tuple<index_t, index_t, value_type>> build_sparse_factor() const;
    std::vector<std::tuple<index_t, index_t, value_type>> build_sparse_diag() const;
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
} // namespace detail

} // namespace CYQLONE_NS(cyqlone)
