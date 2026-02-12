#include <gtest/gtest.h>

#include <cyqlone/linalg.hpp>

#include <tuple>
#include <type_traits>

namespace {

template <class M>
void fill_linear(M &m, double start, double step) {
    using cyqlone::index_t;
    auto *ptr = m.data();
    for (index_t i = 0; i < m.size(); ++i)
        ptr[i] = start + step * static_cast<double>(i);
}

} // namespace

TEST(LinAlg, transformNElementwiseSumDiff) {
    using cyqlone::index_t;
    using vl_t = std::integral_constant<index_t, 4>;
    batmat::matrix::Matrix<double, index_t, vl_t> A{{.depth = 16, .rows = 13, .cols = 17}};
    batmat::matrix::Matrix<double, index_t, vl_t> B{{.depth = 16, .rows = 13, .cols = 17}};
    batmat::matrix::Matrix<double, index_t, vl_t> C{{.depth = 16, .rows = 13, .cols = 17}};
    batmat::matrix::Matrix<double, index_t, vl_t> D{{.depth = 16, .rows = 13, .cols = 17}};
    fill_linear(A, 0.5, 0.25);
    fill_linear(B, -2.0, 0.75);

    const auto fun = [](auto ai, auto bi) { return std::make_tuple(ai + bi, ai - bi); };
    cyqlone::linalg::transform_n_elementwise(fun, std::tie(C, D), A, B);

    for (index_t i = 0; i < A.size(); ++i) {
        const double a = A.data()[i];
        const double b = B.data()[i];
        EXPECT_DOUBLE_EQ(C.data()[i], a + b);
        EXPECT_DOUBLE_EQ(D.data()[i], a - b);
    }
}

TEST(LinAlg, transformNElementwiseInPlaceOutput) {
    using cyqlone::index_t;
    using vl_t = std::integral_constant<index_t, 4>;
    batmat::matrix::Matrix<double, index_t, vl_t> A{{.depth = 8, .rows = 7, .cols = 5}};
    batmat::matrix::Matrix<double, index_t, vl_t> B{{.depth = 8, .rows = 7, .cols = 5}};
    fill_linear(A, 1.0, 0.5);
    fill_linear(B, -3.0, 0.2);
    auto A_ref = A;

    const auto fun = [](auto ai, auto bi) { return std::make_tuple(ai + bi); };
    cyqlone::linalg::transform_n_elementwise(fun, std::tie(A), A, B);

    for (index_t i = 0; i < A.size(); ++i)
        EXPECT_DOUBLE_EQ(A.data()[i], A_ref.data()[i] + B.data()[i]);
}
