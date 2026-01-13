#include <cyqlone/config.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/linalg/gemv.hpp>
#include <batmat/linalg/potrf.hpp>
#include <batmat/linalg/shift.hpp>
#include <batmat/linalg/symv.hpp>
#include <batmat/linalg/syomv.hpp>
#include <batmat/linalg/trsm.hpp>
#include <guanaqo/print.hpp>
#include <iostream>
#include <numeric>
#include <random>

namespace cyqlone {

using namespace batmat::linalg;

// Standalone PCR factorization class for testing
template <index_t VL, class T, StorageOrder DefaultOrder>
class PCRFactorTest {
  public:
    [[nodiscard]] static constexpr index_t get_depth(index_t n) {
        BATMAT_ASSUME(n > 0);
        auto un = static_cast<std::make_unsigned_t<index_t>>(n);
        return static_cast<index_t>(std::bit_width(un - 1));
    }

    using value_type             = T;
    using vl_t                   = std::integral_constant<index_t, VL>;
    using align_t                = std::integral_constant<index_t, VL * alignof(T)>;
    static constexpr index_t vl  = VL;
    static constexpr index_t lvl = get_depth(vl);

    template <StorageOrder O = DefaultOrder>
    using matrix       = batmat::matrix::Matrix<value_type, index_t, vl_t, index_t, O, align_t>;
    using layer_stride = batmat::matrix::DefaultStride;
    template <StorageOrder O = DefaultOrder>
    using batch_view = batmat::matrix::View<const value_type, index_t, vl_t, vl_t, layer_stride, O>;
    template <StorageOrder O = DefaultOrder>
    using mut_batch_view = batmat::matrix::View<value_type, index_t, vl_t, vl_t, layer_stride, O>;

    index_t n;

    matrix<> pcr_L = [this] { return matrix<>{{.depth = VL * (lvl + 1), .rows = n, .cols = n}}; }();
    matrix<> pcr_Y = [this] { return matrix<>{{.depth = VL * lvl, .rows = n, .cols = n}}; }();
    matrix<> pcr_U = [this] { return matrix<>{{.depth = VL * lvl, .rows = n, .cols = n}}; }();
    matrix<> pcr_A = [this] { return matrix<>{{.depth = VL, .rows = n, .cols = n}}; }();
    matrix<> work  = [this] { return matrix<>{{.depth = VL, .rows = n, .cols = 1}}; }();

    void factor_pcr(batch_view<> A, batch_view<> B) {
        potrf(tril(A), tril(pcr_L.batch(0)));
        [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
            (this->template factor_pcr_level<Levels>(A, B), ...);
        }(std::make_integer_sequence<index_t, lvl>{});
    }

    template <index_t Level>
    void factor_pcr_level(batch_view<> A0, batch_view<> B0) {
        static constexpr auto stride = 1 << Level;
        auto A                       = Level == 0 ? A0 : pcr_A.batch(0);
        auto B                       = Level == 0 ? B0 : pcr_Y.batch(Level);
        auto A_next                  = pcr_A.batch(0);
        trsm(B.transposed(), triu(pcr_L.batch(Level).transposed()), pcr_U.batch(Level),
             with_rotate_A<-stride>);
        trsm(B, triu(pcr_L.batch(Level).transposed()), pcr_Y.batch(Level));
        syrk_sub(pcr_U.batch(Level), tril(A), tril(A_next), with_rotate_C<-stride>,
                 with_rotate_D<-stride>, with_mask_D<-stride>);
        syrk_sub(pcr_Y.batch(Level), tril(A_next), with_rotate_C<+stride>, with_rotate_D<+stride>,
                 with_mask_D<+stride>);
        potrf(tril(A_next), tril(pcr_L.batch(Level + 1)));
        if constexpr (Level + 1 < lvl) {
            auto B_next = pcr_Y.batch(Level + 1);
            gemm_neg(pcr_Y.batch(Level), pcr_U.batch(Level).transposed(), B_next, {},
                     with_rotate_C<-stride>, with_rotate_D<-stride>, with_mask_D<-stride>);
        }
    }

    void solve_pcr(mut_batch_view<> λ) { solve_pcr(λ, work.batch(0)); }
    void solve_pcr(mut_batch_view<> λ, mut_batch_view<> work_pcr) const {
        [&]<index_t... Levels>(std::integer_sequence<index_t, Levels...>) {
            (this->template solve_pcr_level<Levels>(λ, work_pcr), ...);
        }(std::make_integer_sequence<index_t, lvl>{});
        trsm(tril(pcr_L.batch(lvl)), λ);
        trsm(triu(pcr_L.batch(lvl).transposed()), λ);
    }

    template <index_t Level>
    void solve_pcr_level(mut_batch_view<> λ, mut_batch_view<> work_pcr) const {
        static constexpr auto stride = 1 << Level;
        trsm(tril(pcr_L.batch(Level)), λ, work_pcr); // w = L⁻¹ λ
        gemv_sub(pcr_Y.batch(Level), work_pcr, λ, with_rotate_C<+stride>, with_rotate_D<+stride>,
                 with_mask_D<+stride>);
        gemv_sub(pcr_U.batch(Level), work_pcr, λ, with_rotate_C<-stride>, with_rotate_D<-stride>,
                 with_mask_D<-stride>);
    }
};

} // namespace cyqlone

template <class M>
void print_matrix_batches(std::string_view name, const M &matrix) {
    std::cout << name << (matrix.num_batches() == 1 ? " = " : " = [");
    for (typename M::index_type l = 0; l < matrix.num_batches(); ++l) {
        auto L = matrix.batch(l);
        std::cout << "[\n";
        for (typename M::index_type i = 0; i < L.depth(); ++i)
            guanaqo::print_python(std::cout, L(i), ",\n", false);
        std::cout << (l + 1 < matrix.num_batches() ? "],\n" : "]");
    }
    std::cout << (matrix.num_batches() == 1 ? "\n\n" : "]\n\n");
}

int main() {
    using std::abs;
    using namespace cyqlone;
    using T = double;
#if BATMAT_HAS_DOUBLE_VL_8
    constexpr index_t N = 8; // number of blocks
#elif BATMAT_HAS_DOUBLE_VL_4
    constexpr index_t N = 4; // number of blocks
#else
#error "No suitable vector length available"
#endif
    constexpr index_t n = 5; // block size

    using Solver = PCRFactorTest<N, T, StorageOrder::ColMajor>;

    Solver::matrix<> A{{.depth = N, .rows = n, .cols = n}}; // diagonal blocks
    Solver::matrix<> B{{.depth = N, .rows = n, .cols = n}}; // subdiagonal blocks
    Solver::matrix<> c{{.depth = N, .rows = n, .cols = 1}}; // right-hand side

    std::mt19937 rng(12345);
    std::uniform_real_distribution<T> dist(-1.0, 1.0);
    std::ranges::generate(B, [&] { return dist(rng); });
    syrk(B.batch(0), tril(A.batch(0)));
    std::ranges::generate(B, [&] { return 1e-3 * dist(rng); });
    B(N - 1).set_constant(0); // not circular
    std::ranges::generate(c, [&] { return dist(rng); });

    Solver solver{.n = n};

    solver.factor_pcr(A.batch(0), B.batch(0));
    Solver::matrix<> x = c;
    solver.solve_pcr(x.batch(0));

    Solver::matrix<> r{{.depth = N, .rows = n, .cols = 1}}; // residual
    syomv(tril(B.batch(0)), x.batch(0), r.batch(0));
    symv_add(tril(A.batch(0)), x.batch(0), r.batch(0));
    auto res_norm = std::inner_product(
        r.data(), r.data() + r.size(), c.data(), T{0}, [](T a, T b) { return std::fmax(a, b); },
        [](T r, T c) { return abs(r - c); });
    std::cout << "Residual: " << res_norm << "\n\n";

    // Print results
    std::cout << "n = " << n << "\n";
    std::cout << "N = " << N << "\n";
    print_matrix_batches("A", A);
    print_matrix_batches("B", B);
    print_matrix_batches("c", c);
    print_matrix_batches("L", solver.pcr_L);
    print_matrix_batches("Y", solver.pcr_Y);
    print_matrix_batches("U", solver.pcr_U);
    print_matrix_batches("x", x);

    return res_norm < 1e-10 ? 0 : 1;
}
