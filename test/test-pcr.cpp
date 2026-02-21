#include <cyqlone/config.hpp>
#include <cyqlone/linalg.hpp>
#include <cyqlone/matio.hpp>
#include <cyqlone/packing.hpp>
#include <batmat/dtypes.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/symv.hpp>
#include <batmat/linalg/syomv.hpp>
#include <batmat/linalg/trsm.hpp>
#include <guanaqo/print.hpp>
#include <cmath>
#include <iostream>
#include <random>

namespace cyqlone {

using namespace batmat::linalg;
using namespace cyqlone::linalg;

[[nodiscard]] constexpr index_t get_depth(index_t n) {
    return static_cast<index_t>(std::bit_width(static_cast<std::make_unsigned_t<index_t>>(n) - 1));
}

// Standalone PCR factorization class for testing
template <index_t VL, class T, StorageOrder DefaultOrder>
struct PCRFactorTest {
    using value_type            = T;
    using vl_t                  = std::integral_constant<index_t, VL>;
    using align_t               = std::integral_constant<index_t, VL * alignof(T)>;
    static constexpr index_t v  = VL;
    static constexpr index_t lv = get_depth(v);

    template <StorageOrder O = DefaultOrder>
    using bmatrix = batmat::matrix::Matrix<value_type, index_t, vl_t, index_t, O, align_t>;
    template <StorageOrder O = DefaultOrder>
    using matrix = batmat::matrix::Matrix<value_type, index_t, vl_t, vl_t, O, align_t>;
    template <StorageOrder O = DefaultOrder>
    using mut_view = matrix<O>::view_type;
    template <StorageOrder O = DefaultOrder>
    using view = matrix<O>::const_view_type;

    index_t n;

    bmatrix<> pcr_L = [this] { return bmatrix<>{{.depth = v * (lv + 1), .rows = n, .cols = n}}; }();
    bmatrix<> pcr_Y = [this] { return bmatrix<>{{.depth = v * lv, .rows = n, .cols = n}}; }();
    bmatrix<> pcr_U = [this] { return bmatrix<>{{.depth = v * lv, .rows = n, .cols = n}}; }();
    matrix<> pcr_M  = [this] { return matrix<>{{.rows = n, .cols = n}}; }();
    matrix<> work   = [this] { return matrix<>{{.rows = n, .cols = 1}}; }();

    void factor_pcr(view<> M0, view<> K0) {
        [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
            (this->template factor_pcr_level<Levels>(M0, K0), ...);
        }(std::make_integer_sequence<index_t, lv>{});
        auto M = lv == 0 ? M0 : pcr_M.batch(0);
        auto L = pcr_L.batch(lv);
        potrf(tril(M), tril(L));
    }

    template <index_t l>
    void factor_pcr_level(view<> M0, view<> K0) {
        static constexpr auto r = 1 << l;
        auto M = l == 0 ? M0 : pcr_M.batch(0), K = l == 0 ? K0 : pcr_Y.batch(l);
        auto L = pcr_L.batch(l), Y = pcr_Y.batch(l), U = pcr_U.batch(l);
        auto M_next = pcr_M.batch(0);
        potrf(tril(M), tril(L));
        if constexpr (l + 1 < lv) {
            auto K_next = pcr_Y.batch(l + 1);
            trsm(K.transposed(), triu(L.transposed()), U, with_rotate_A<-r>);
            trsm(K, triu(L.transposed()), Y);
            syrk_sub(U, tril(M), tril(M_next), with_rotate_C<-r>, with_rotate_D<-r>);
            syrk_sub(Y, tril(M_next), with_rotate_C<+r>, with_rotate_D<+r>);
            gemm_neg(Y, U.transposed(), K_next, {}, with_rotate_C<-r>, with_rotate_D<-r>);
        } else {
            copy(K, U, with_rotate<+r>);
            add(U, K.transposed()); // U contains Kᵀ now
            trsm(U, triu(L.transposed()), with_rotate_A<-r>);
            syrk_sub(U, tril(M), tril(M_next), with_rotate_C<-r>, with_rotate_D<-r>);
        }
    }

    void solve_pcr(mut_view<> λ) { solve_pcr(λ, work.batch(0)); }
    void solve_pcr(mut_view<> λ, mut_view<> work_pcr) const {
        [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
            (this->template solve_pcr_level<Levels>(λ, work_pcr), ...);
        }(std::make_integer_sequence<index_t, lv>{});
        trsm(tril(pcr_L.batch(lv)), λ);
        trsm(triu(pcr_L.batch(lv).transposed()), λ);
    }

    template <index_t l>
    void solve_pcr_level(mut_view<> λ, mut_view<> work_pcr) const {
        static constexpr auto r = 1 << l;
        trsm(tril(pcr_L.batch(l)), λ, work_pcr); // w = L⁻¹ λ
        gemv_sub(pcr_U.batch(l), work_pcr, λ, with_rotate_C<-r>, with_rotate_D<-r>);
        if constexpr (l + 1 < lv)
            gemv_sub(pcr_Y.batch(l), work_pcr, λ, with_rotate_C<+r>, with_rotate_D<+r>);
    }
};

// Quick-and-dirty power iteration to estimate the spectral norm of a block tridiagonal matrix.
template <class Mat>
auto λ_max_power(const Mat &M, const Mat &K, int max_it, typename Mat::value_type tol) {
    using std::abs;
    using std::sqrt;
    using T = typename Mat::value_type;
    batmat::matrix::Matrix<typename Mat::value_type, typename Mat::index_type,
                           typename Mat::batch_size_type, typename Mat::depth_type>
        x{{.rows = M.rows(), .cols = 1}}, Ax{{.rows = M.rows(), .cols = 1}};
    x.set_constant(T(1) / sqrt(T(M.rows())));
    T λ = 0, λ_old = 0;
    int k;
    for (k = 0; k < max_it; ++k) {
        symv(tril(M), x, Ax);                     // y = M x
        syomv(tril(K), x, Ax);                    // y += K x
        λ_old     = std::exchange(λ, dot(x, Ax)); // λ = xᵀAx (Rayleigh quotient, ‖x‖=1)
        auto norm = norm_2(Ax);                   // ‖Ax‖
        scale(T(1) / norm, Ax, x);                // x ← Ax/‖Ax‖
        if (abs(λ - λ_old) < tol * abs(λ))
            break;
    }
    return std::make_pair(λ, k + 1);
};

template <class M>
auto unpacked(const M &matrix) {
    batmat::matrix::Matrix<typename M::value_type, typename M::index_type> res{
        {.depth = matrix.depth(), .rows = matrix.rows(), .cols = matrix.cols()}};
    unpack(matrix, res);
    return res;
}

} // namespace cyqlone

int main() {
    using std::abs;
    using std::sqrt;
    using namespace cyqlone;
    using T             = batmat::real_t;
    constexpr index_t N = batmat::types::vl_or_largest<T, 8>; // number of blocks == vector length
    static_assert(N > 1, "No suitable vector length available");
    using Solver        = PCRFactorTest<N, T, StorageOrder::ColMajor>;
    constexpr index_t n = 15; // block size

    Solver::matrix<> A{{.rows = n, .cols = n}}, B{{.rows = n, .cols = n}}; // (for initialization)
    Solver::matrix<> M{{.rows = n, .cols = n}}; // diagonal blocks of the block tridiagonal system
    Solver::matrix<> K{{.rows = n, .cols = n}}; // subdiagonal blocks
    Solver::matrix<> b{{.rows = n, .cols = 1}}; // right-hand side

    // Generate random block tridiagonal system
    std::mt19937 rng(12345);
    std::uniform_real_distribution<T> dist(-1, 1);
    std::ranges::generate(A, [&] { return dist(rng); });
    std::ranges::generate(B, [&] { return dist(rng); });
    std::ranges::generate(b, [&] { return dist(rng); });
    // M(i) = A(i) A(i)ᵀ + B(i-1) B(i-1)ᵀ and K(i) = B(i) A(i)ᵀ
    syrk(A, tril(M));
    syrk_add(B, tril(M), with_rotate_C<1>, with_rotate_D<1>);
    gemm(B, A.transposed(), K);
    M.add_to_diagonal(T(1e-4) * static_cast<T>(n * N)); // ensure positive definiteness

    // PCR factorization and solution
    Solver solver{.n = n};
    solver.factor_pcr(M, K);
    Solver::matrix<> x = b;
    solver.solve_pcr(x);

    // Residual M x - b
    Solver::matrix<> r{{.rows = n, .cols = 1}};
    cyqlone::linalg::negate(b, r);
    symv_add(tril(M), x, r);
    syomv(tril(K), x, r);
    auto res_norm = cyqlone::linalg::norms_all(r);
    std::cout << "\nResidual norms: ℓ₂ = " << guanaqo::float_to_str(res_norm.norm2())
              << ",\tmax = " << guanaqo::float_to_str(res_norm.norminf()) << "\n";
    constexpr auto ε = std::numeric_limits<T>::epsilon();
    const int max_it = 100 * N * n;
    auto [normM, it] = λ_max_power(M, K, max_it, ε);
    std::cout << "Spectral norm M: " << guanaqo::float_to_str(normM) << " ("
              << (it <= max_it ? "" : "approx., ") << it << " iter.)\n";
    using cyqlone::linalg::norm_2;
    const auto η = res_norm.norm2() / (normM * norm_2(x) + norm_2(b));
    std::cout << "Backward error: " << guanaqo::float_to_str(η) << "\n\n";

#if CYQLONE_WITH_MATIO
    // Export the original system and the solution as a .mat file
    std::filesystem::path filename = "test-pcr.mat";
    auto matfile                   = cyqlone::create_mat(filename);
    cyqlone::add_to_mat(matfile.get(), "A", unpacked(A));
    cyqlone::add_to_mat(matfile.get(), "B", unpacked(B));
    cyqlone::add_to_mat(matfile.get(), "M", unpacked(M));
    cyqlone::add_to_mat(matfile.get(), "K", unpacked(K));
    cyqlone::add_to_mat(matfile.get(), "b", unpacked(b));
    cyqlone::add_to_mat(matfile.get(), "x", unpacked(x));
    std::cout << "Saved system and solution to " << filename << "\n";
#endif

    return η < ε * static_cast<T>(100 * n * N) ? 0 : 1;
}
