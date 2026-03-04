#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <cmath>
#include <limits>

#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/trtri.hpp>

namespace CYQLONE_NS(cyqlone) {
using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
auto CyqloneSolver<VL, T, DefaultOrder, Ctx>::build_sparse(const CyqloneStorage<value_type> &ocp,
                                                           std::span<const value_type> Σ) const
    -> SparseMatrix {
    BATMAT_ASSERT(is_pow_2(p));
    using enum batmat::linalg::MatrixStructure;
    using std::sqrt;
    const index_t nux = nu + nx, nuxx = nux + nx;
    // stride between stages in the same interval
    const index_t stage_stride = nuxx;
    // stride between intervals in the thread partitioning
    const index_t interval_stride = nuxx * n - nx;
    // stride between vector lanes (largest stride)
    const index_t vector_stride = p * interval_stride;
    // total matrix size
    const index_t nn = ceil_N() * nuxx;
    // first index of the CR block
    const index_t sλ = nn - (nx * p * v);

    SparseMatrixBuilder mat{.rows = nn, .cols = nn, .symmetry = Symmetry::Unsymmetric};

    // Add the ALM penalty terms to the cost Hessians
    batmat::matrix::Matrix<value_type, index_t> H{{.depth = N_horiz, .rows = nux, .cols = nux}};
    const auto nyM = std::max(ny, ny_0 + ny_N);
    batmat::matrix::Matrix<value_type, index_t> DC{{.depth = N_horiz, .rows = nyM, .cols = nux}};
    auto R  = H.top_left(nu, nu);
    auto Q  = H.bottom_right(nx, nx);
    auto Sᵀ = H.bottom_left(nx, nu);
    for (index_t j = 0; j < N_horiz; ++j) {
        H(j) = ocp.data_H(j);
        if (j > 0) {
            DC.top_rows(ny)(j) = ocp.data_G(j - 1);
            for (index_t r = 0; r < ny; ++r)
                for (index_t c = 0; c < nux; ++c)
                    DC(j, r, c) *= sqrt(Σ[ny_0 + (j - 1) * ny + r]);
            guanaqo::blas::xsyrk_LT(value_type{1}, DC(j), value_type{1}, H(j));
        } else {
            DC.top_rows(ny_0 + ny_N)(j) = ocp.data_G0N(0);
            for (index_t r = 0; r < ny_0; ++r)
                for (index_t c = 0; c < nu; ++c)
                    DC(0, r, c) *= sqrt(Σ[r]);
            for (index_t r = 0; r < ny_N; ++r)
                for (index_t c = 0; c < nx; ++c)
                    DC(0, ny_0 + r, nu + c) *= sqrt(Σ[ny_0 + (N_horiz - 1) * ny + r]);
            guanaqo::blas::xsyrk_LT(value_type{1}, DC(j), value_type{1}, H(j));
        }
    }

    // Populate the sparse matrix
    for (index_t l = 0; l < v; ++l) { // vector lane
        for (index_t c = 0; c < p; ++c) {
            const index_t j0 = c * n + l * p * n;
            const auto biA   = c + l * p;
            const auto biI   = sub_wrap_ceil_P(biA, 1);
            const auto sλA   = sλ + nx * get_linear_batch_offset(biA);
            const auto sλI   = sλ + nx * get_linear_batch_offset(biI);
            // TODO: handle case if lev > or >= lp()
            for (index_t i = 0; i < n; ++i) {
                const index_t j = sub_wrap_ceil_N(j0, i);
                // index of current diagonal block
                index_t s = l * vector_stride + c * interval_stride + stage_stride * i;
                // Padding
                if (j >= N_horiz) {
                    mat.add_diag(s, s, 1, nux);                         // H = I
                    i + 1 == n ? mat.add_diag(sλI, s + nu, -1, nx)      // E = -I
                               : mat.add_diag(s + nux, s + nu, -1, nx); // E = -I
                    continue;
                }
                // H
                mat.add(s, s, R(j), 1, LowerTriangular);
                mat.add(s + nu, s + nu, Q(j), 1, LowerTriangular);
                mat.add(s + nu, s, Sᵀ(j));
                // beginning of subinterval: coupling constraints at the bottom (row sλA)
                if (i == 0) {
                    mat.add(sλA, s, ocp.data_F(j).left_cols(nu));              // B(j)
                    j > 0 ? mat.add(sλA, s + nu, ocp.data_F(j).right_cols(nx)) // A(j)
                          : void();                                            // A(0) = 0
                } else {
                    mat.add(s, s - nx, ocp.data_F(j).left_cols(nu).transposed());       // B(j)
                    mat.add(s + nu, s - nx, ocp.data_F(j).right_cols(nx).transposed()); // A(j)
                }
                // end of subinterval: coupling constraints at the bottom (row sλI)
                i + 1 == n ? mat.add_diag(sλI, s + nu, -1, nx)      // E = -I
                           : mat.add_diag(s + nux, s + nu, -1, nx); // E = -I
            }
        }
    }
    return std::move(mat).build();
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
auto CyqloneSolver<VL, T, DefaultOrder, Ctx>::build_rhs(view<> rq, view<> b, value_type scale_rq,
                                                        value_type scale_b) const
    -> std::vector<T> {
    BATMAT_ASSERT(is_pow_2(p));
    const index_t nux = nu + nx, nuxx = nux + nx;
    // stride between stages in the same interval
    const index_t stage_stride = nuxx;
    // stride between intervals in the thread partitioning
    const index_t interval_stride = nuxx * n - nx;
    // stride between vector lanes (largest stride)
    const index_t vector_stride = p * interval_stride;
    // total vector size
    const index_t nn = ceil_N() * nuxx;
    // first index of the CR block
    const index_t sλ = nn - (nx * p * v);

    std::vector<value_type> rhs(nn);
    std::ranges::fill(rhs, std::numeric_limits<value_type>::quiet_NaN());

    for (index_t l = 0; l < v; ++l) {
        for (index_t t = 0; t < p; ++t) {
            const index_t di0 = t * n;
            for (index_t i = 0; i < n; ++i) {
                const index_t di = di0 + i;
                index_t s        = l * vector_stride + t * interval_stride + stage_stride * i;
                if (i > 0)
                    for (index_t c = 0; c < nx; ++c)
                        rhs[s - nx + c] = scale_b * b.batch(di)(l)(c, 0);
                for (index_t c = 0; c < nux; ++c)
                    rhs[s + c] = scale_rq * rq.batch(di)(l)(c, 0);
            }
        }
    }
    index_t s               = sλ;
    const auto cyclic_block = [&](index_t i) {
        const index_t t = i % p, l = i / p;
        const index_t di = t * n;
        for (index_t c = 0; c < nx; ++c)
            rhs[s + c] = scale_b * b.batch(di)(l)(c, 0);
        s += nx;
    };
    for (index_t l = 0; l < lp(); ++l) {
        index_t offset = 1 << l;
        index_t stride = offset << 1;
        for (index_t i = offset; i < v * p; i += stride)
            cyclic_block(i);
    }
    for (index_t i = 0; i < v * p; i += p) {
        cyclic_block(i);
    }
    return rhs;
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
auto CyqloneSolver<VL, T, DefaultOrder, Ctx>::build_sparse_factor() const -> SparseMatrix {
    BATMAT_ASSERT(is_pow_2(p));
    using enum batmat::linalg::MatrixStructure;
    const index_t nux = nu + nx, nuxx = nux + nx;
    // stride between stages in the same interval
    const index_t stage_stride = nuxx;
    // stride between intervals in the thread partitioning
    const index_t interval_stride = nuxx * n - nx;
    // stride between vector lanes (largest stride)
    const index_t vector_stride = p * interval_stride;
    // total matrix size
    const index_t nn = ceil_N() * nuxx;
    // first index of the CR block
    const index_t sλ = nn - (nx * p * v);

    SparseMatrixBuilder mat{.rows = nn, .cols = nn, .symmetry = Symmetry::Unsymmetric};

    // Compute blocks that are not computed by the Cyqlone factorization (because they are not
    // needed for the solve)
    matrix<> LA{{.depth = v * p, .rows = nx, .cols = n * nx}};
    matrix<> invQᵀ{{.depth = v * p, .rows = nx, .cols = n * nx}};
    matrix<> LBA{{.depth = v * p, .rows = nu + nx, .cols = (n - 1) * nx}};
    for (index_t t = 0; t < p; ++t) {
        const index_t di0 = t * n; // data batch index
        auto RSQ          = riccati_LH.batch(t);
        auto LBAt         = LBA.batch(t);
        auto invQᵀt       = invQᵀ.batch(t);
        auto Â            = riccati_LAB.batch(t).left_cols(n * nx);
        auto LAt          = LA.batch(t);
        for (index_t i = 0; i < n; ++i) {
            const auto di = di0 + i;
            auto RSQi     = RSQ.middle_cols(i * nux, nux);
            auto Qi       = tril(RSQi.bottom_right(nx, nx));
            auto Qi_inv   = triu(invQᵀt.middle_cols(i * nx, nx));
            auto Âi       = Â.middle_cols(i * nx, nx);
            auto LAi      = LAt.middle_cols(i * nx, nx);

            copy(Âi, LAi);
            if (i + 1 < n) // Final block already inverted
                trtri(Qi, Qi_inv.transposed());
            else
                copy(triu(RSQi.block(nu - 1, nu, nx, nx)), Qi_inv);
            if (i + 1 < n) // Final block is already Â LQ⁻ᵀ
                trsm(LAi, Qi.transposed());
            if (i > 0) {
                auto LBAi     = LBAt.middle_cols((i - 1) * nx, nx);
                auto RSQ_prev = RSQ.middle_cols((i - 1) * nux, nux);
                auto Q_prev   = tril(RSQ_prev.bottom_right(nx, nx));
                auto BA       = data_F.batch(di);
                copy(BA.transposed(), LBAi);
                trmm(LBAi, Q_prev);
            }
        }
    }

    // Populate the sparse matrix
    for (index_t l = 0; l < v; ++l) {
        for (index_t t = 0; t < p; ++t) {
            const index_t j0 = t * n + l * n * p;
            const auto biA   = t + l * p;
            const auto biI   = sub_wrap_ceil_P(biA, 1);
            const auto sλA   = sλ + nx * get_linear_batch_offset(biA);
            const auto sλI   = sλ + nx * get_linear_batch_offset(biI);
            auto B̂t          = riccati_LAB.batch(t).right_cols(n * nu);
            auto LHt         = riccati_LH.batch(t);
            auto LBAt        = LBA.batch(t);
            auto LAt         = LA.batch(t);
            auto invQᵀt      = invQᵀ.batch(t);
            for (index_t i = 0; i < n; ++i) {
                [[maybe_unused]] const index_t j = sub_wrap_ceil_N(j0, i);
                index_t s  = l * vector_stride + t * interval_stride + i * stage_stride;
                auto B̂i    = B̂t.middle_cols(i * nu, nu);
                auto LHi   = LHt.middle_cols(i * nux, nux);
                auto iQiᵀ  = invQᵀt.middle_cols(i * nx, nx);
                auto AiQiᵀ = LAt.middle_cols(i * nx, nx);
                if (i > 0) {
                    auto LBAi    = LBAt.middle_cols((i - 1) * nx, nx);
                    auto iQᵀprev = invQᵀt.middle_cols((i - 1) * nx, nx);
                    auto LA_prev = LAt.middle_cols((i - 1) * nx, nx);
                    mat.add(s - nx, s - nx, iQᵀprev(l), -1, UpperTriangular);
                    mat.add(s, s - nx, LBAi(l));
                    mat.add(sλA, s - nx, LA_prev(l));
                }
                mat.add(s, s, LHi(l), 1, LowerTriangular);
                mat.add(sλA, s, B̂i(l));
                if (i + 1 < n)
                    mat.add(s + nux, s + nu, iQiᵀ(l), -1, UpperTriangular);
                else
                    mat.add(sλI, s + nu, iQiᵀ(l), -1, UpperTriangular);
                mat.add(sλA, s + nu, AiQiᵀ(l));
            }
        }
    }
    index_t s               = sλ;
    const auto cyclic_block = [&](index_t i, index_t offset) {
        const index_t sY = sλ + nx * get_linear_batch_offset(i + offset);
        const index_t sU = sλ + nx * get_linear_batch_offset(i - offset);
        const index_t t = i % p, l = i / p;
        mat.add(s, s, tricyqle.cr_L.batch(t)(l), 1, LowerTriangular);
        if (i + offset < v * p)
            mat.add(sY, s, tricyqle.cr_Y.batch(t)(l));
        mat.add(sU, s, tricyqle.cr_U.batch(t)(l));
        s += nx;
    };
    const auto cyclic_block_final = [&](index_t i, index_t offset) {
        const index_t sY = sλ + nx * get_linear_batch_offset(i + offset);
        const index_t t = i % p, l = i / p;
        mat.add(s, s, tricyqle.pcr_L.batch(0)(l), 1, LowerTriangular);
        if (i + offset < v * p)
            mat.add(sY, s, tricyqle.cr_Y.batch(t)(l));
        s += nx;
    };
    for (index_t l = 0; l < lp(); ++l) {
        index_t offset = 1 << l;
        index_t stride = offset << 1;
        for (index_t i = offset; i < v * p; i += stride)
            cyclic_block(i, offset);
    }
    for (index_t i = 0; i < v * p; i += p)
        cyclic_block_final(i, p);
    return std::move(mat).build();
}

template <index_t VL, class T, StorageOrder DefaultOrder, class Ctx>
auto CyqloneSolver<VL, T, DefaultOrder, Ctx>::build_sparse_diag() const -> SparseMatrix {
    BATMAT_ASSERT(is_pow_2(p));
    const index_t nux = nu + nx, nuxx = nux + nx;
    // stride between stages in the same interval
    const index_t stage_stride = nuxx;
    // stride between intervals in the thread partitioning
    const index_t interval_stride = nuxx * n - nx;
    // stride between vector lanes (largest stride)
    const index_t vector_stride = p * interval_stride;
    // total matrix size
    const index_t nn = ceil_N() * nuxx;
    // first index of the CR block
    const index_t sλ = nn - (nx * p * v);

    SparseMatrixBuilder mat{.rows = nn, .cols = nn, .symmetry = Symmetry::Lower};

    for (index_t l = 0; l < v; ++l) {
        for (index_t t = 0; t < p; ++t) {
            for (index_t i = 0; i < n; ++i) {
                index_t s = l * vector_stride + t * interval_stride + i * stage_stride;
                if (i > 0)
                    for (index_t c = 0; c < nx; ++c)
                        mat.add(s - nx + c, s - nx + c, -1);
                for (index_t c = 0; c < nu; ++c)
                    mat.add(s + c, s + c, 1);
                for (index_t c = 0; c < nx; ++c)
                    mat.add(s + c + nu, s + c + nu, 1);
            }
        }
    }
    for (index_t i = 0; i < p * v; ++i)
        for (index_t r = 0; r < nx; ++r)
            mat.add(sλ + nx * i + r, sλ + nx * i + r, -1);
    return std::move(mat).build();
}

} // namespace CYQLONE_NS(cyqlone)
