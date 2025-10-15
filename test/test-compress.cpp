#include <cyqlone/config.hpp>
#include <cyqlone/implementation/compress.hpp>
#include <Eigen/Core>
#include <batmat/config.hpp>
#include <batmat/linalg/gemm-diag.hpp>
#include <batmat/linalg/gemm.hpp>
#include <batmat/matrix/matrix.hpp>
#include <gtest/gtest.h>
#include <guanaqo/eigen/view.hpp>
#include <limits>
#include <random>

#include "eigen-matchers.hpp"

using batmat::linalg::tril;
using cyqlone::index_t;
using cyqlone::real_t;
using I4 = std::integral_constant<index_t, 4>;

TEST(Compress, sqrt) {
    using Mat = batmat::matrix::Matrix<real_t, index_t, I4, I4>;
    Mat A{{.rows = 5, .cols = 231}}, Acompact{{.rows = 5, .cols = 231}};
    Mat S{{.rows = 231, .cols = 1}};
    std::mt19937_64 rng{12345};
    std::uniform_real_distribution<real_t> dist{0, 1};
    std::ranges::generate(A.begin(), A.end(), [&]() { return dist(rng); });
    std::ranges::generate(S.begin(), S.end(), [&]() { return dist(rng); });
    std::bernoulli_distribution bdist{0.333};
    for (auto &s : S)
        if (bdist(rng))
            s = 0;
    Mat ASAᵀ{{.rows = 5, .cols = 5}}, ASAᵀcompact{{.rows = 5, .cols = 5}};
    syrk_diag_add(A, tril(ASAᵀ), S);
    auto m_compact = cyqlone::linalg::compress_masks_sqrt(A, S, Acompact);
    syrk_add(Acompact.left_cols(m_compact), tril(ASAᵀcompact));

    const auto ε = std::numeric_limits<real_t>::epsilon() * 100;
    for (index_t l = 0; l < ASAᵀ.depth(); ++l) {
        EXPECT_THAT(as_eigen(ASAᵀcompact(l)), EigenAlmostEqual(as_eigen(ASAᵀ(l)), ε))
            << "at layer " << l;
    }
}
