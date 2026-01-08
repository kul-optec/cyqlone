#include <cyqlone/v2/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <cmath>
#include <limits>

#include <batmat/linalg/copy.hpp>
#include <batmat/linalg/trtri.hpp>

namespace CYQLONE_NS(cyqlone)::v2 {
using namespace batmat::linalg;

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::build_sparse(const CyqloneStorage<value_type> &ocp,
                                                      std::span<const value_type> Σ) const
    -> SparseMatrix {
    using std::sqrt;
    const index_t nux = nu + nx, nuxx = nux + nx;
    const index_t vstride    = p * n;
    const index_t num_stages = n; // number of stages per thread
    const index_t nn         = ceil_N() * nuxx;
    const index_t sλ         = nn - (nx * p * vl);
    SparseMatrixBuilder mat{.rows = nn, .cols = nn, .symmetry = Symmetry::Unsymmetric};

    batmat::matrix::Matrix<value_type, index_t> RSQ_DC{{
        .depth = N_horiz,
        .rows  = nux,
        .cols  = nux,
    }};
    batmat::matrix::Matrix<value_type, index_t> DC{{
        .depth = N_horiz,
        .rows  = std::max(ny, ny_0 + ny_N),
        .cols  = nux,
    }};
    auto R  = RSQ_DC.top_left(nu, nu);
    auto Q  = RSQ_DC.bottom_right(nx, nx);
    auto Sᵀ = RSQ_DC.bottom_left(nx, nu);
    for (index_t k = 0; k < N_horiz; ++k) {
        RSQ_DC(k) = ocp.data_H(k);
        if (k > 0) {
            DC.top_rows(ny)(k) = ocp.data_G(k - 1);
            for (index_t j = 0; j < ny; ++j)
                for (index_t i = 0; i < nux; ++i)
                    DC(k, j, i) *= sqrt(Σ[ny_0 + (k - 1) * ny + j]);
            guanaqo::blas::xsyrk_LT(value_type{1}, DC(k), value_type{1}, RSQ_DC(k));
        } else {
            DC.top_rows(ny_0 + ny_N)(k) = ocp.data_G0N(0);
            for (index_t j = 0; j < ny_0; ++j)
                for (index_t i = 0; i < nu; ++i)
                    DC(0, j, i) *= sqrt(Σ[j]);
            for (index_t j = 0; j < ny_N; ++j)
                for (index_t i = 0; i < nx; ++i)
                    DC(0, ny_0 + j, nu + i) *= sqrt(Σ[ny_0 + (N_horiz - 1) * ny + j]);
            guanaqo::blas::xsyrk_LT(value_type{1}, DC(k), value_type{1}, RSQ_DC(k));
        }
    }
    for (index_t vi = 0; vi < vl; ++vi) {
        const index_t sv = vi * p * (nuxx * num_stages - nx);
        for (index_t ti = 0; ti < p; ++ti) {
            const index_t k0 = ti * num_stages + vi * vstride;
            const auto biA   = ti + vi * p;
            const auto biI   = sub_wrap_P(biA, 1);
            const auto sλA   = sλ + nx * get_linear_batch_offset(biA);
            const auto sλI   = sλ + nx * get_linear_batch_offset(biI);
            // TODO: handle case if lev > or >= lp()
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t k = sub_wrap_N(k0, i);
                index_t s       = sv + ti * (nuxx * num_stages - nx) + nuxx * i;
                if (k >= N_horiz) {
                    for (index_t c = 0; c < nu; ++c)
                        mat.add(s + c, s + c, 1); // R = I
                    for (index_t c = 0; c < nx; ++c) {
                        mat.add(s + c + nu, s + c + nu, 1); // Q = I
                        if (i + 1 < num_stages)
                            mat.add(s + c + nux, s + c + nu, -1); // E = -I
                        else
                            mat.add(sλI + c, s + c + nu, -1); // E = -I
                    }
                    continue;
                }
                for (index_t c = 0; c < nu; ++c) {
                    for (index_t r = c; r < nu; ++r)
                        mat.add(s + r, s + c, R(k)(r, c));
                    if (k > 0)
                        for (index_t r = 0; r < nx; ++r)
                            mat.add(s + r + nu, s + c, Sᵀ(k)(r, c));
                    if (i == 0)
                        for (index_t r = 0; r < nx; ++r)
                            mat.add(sλA + r, s + c, ocp.data_F(k0).left_cols(nu)(r, c));
                }
                for (index_t c = 0; c < nx; ++c) {
                    for (index_t r = c; r < nx; ++r)
                        mat.add(s + r + nu, s + c + nu, Q(k)(r, c));
                    if (i + 1 < num_stages)
                        mat.add(s + c + nux, s + c + nu, -1);
                    else
                        mat.add(sλI + c, s + c + nu, -1);
                    if (i == 0 && k > 0)
                        for (index_t r = 0; r < nx; ++r)
                            mat.add(sλA + r, s + c + nu, ocp.data_F(k0).right_cols(nx)(r, c));
                }
                if (i > 0) {
                    for (index_t c = 0; c < nx; ++c) {
                        for (index_t r = 0; r < nu; ++r)
                            mat.add(s + r, s - nx + c, ocp.data_F(k).left_cols(nu)(c, r));
                        for (index_t r = 0; r < nx; ++r)
                            mat.add(s + nu + r, s - nx + c, ocp.data_F(k).right_cols(nx)(c, r));
                    }
                }
            }
        }
    }
    return std::move(mat).build();
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::build_rhs(view<> ux, view<> λ) const -> std::vector<T> {
    const index_t nux = nu + nx, nuxx = nux + nx;
    std::vector<value_type> rhs(nuxx * ceil_N());
    std::ranges::fill(rhs, std::numeric_limits<value_type>::quiet_NaN());
    const index_t num_stages = n; // number of stages per thread
    const index_t num_proc   = p;
    const index_t sλ         = ceil_N() * nuxx - (nx * p * vl);

    for (index_t vi = 0; vi < vl; ++vi) {
        const index_t sv = vi * num_proc * (nuxx * num_stages - nx);
        for (index_t ti = 0; ti < num_proc; ++ti) {
            const index_t di0 = ti * num_stages;
            for (index_t i = 0; i < num_stages; ++i) {
                const index_t di = di0 + i;
                index_t s        = sv + ti * (nuxx * num_stages - nx) + nuxx * i;
                if (i > 0)
                    for (index_t c = 0; c < nx; ++c)
                        rhs[s - nx + c] = λ.batch(di)(vi)(c, 0);
                for (index_t c = 0; c < nux; ++c)
                    rhs[s + c] = ux.batch(di)(vi)(c, 0);
            }
        }
    }
    index_t s               = sλ;
    const auto cyclic_block = [&](index_t i) {
        const index_t bi = i % p;
        const index_t vi = i / p;
        const index_t di = bi * num_stages;
        for (index_t c = 0; c < nx; ++c)
            rhs[s + c] = λ.batch(di)(vi)(c, 0);
        s += nx;
    };
    if (lP != lvl) {
        for (index_t i = 0; i < ((p * vl) >> 1); ++i)
            cyclic_block(2 * i + 1);
        for (index_t l = 1; l < lp(); ++l) {
            index_t offset = 1 << l;
            index_t stride = offset << 1;
            for (index_t i = offset; i < vl * p; i += stride)
                cyclic_block(i);
        }
    }
    for (index_t i = 0; i < vl * p; i += p) {
        cyclic_block(i);
    }
    return rhs;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::build_sparse_factor() const -> SparseMatrix {
    constexpr bool alt = true;
    const index_t nux = nu + nx, nuxx = nux + nx;
    const index_t vstride    = n * p;
    const index_t num_stages = n; // number of stages per thread
    const index_t num_proc   = p;
    const index_t nn         = ceil_N() * nuxx;
    const index_t sλ         = nn - (nx * p * vl);
    SparseMatrixBuilder mat{.rows = nn, .cols = nn, .symmetry = Symmetry::Unsymmetric};
    matrix AinvQᵀ{{
        .depth = vl * p,
        .rows  = nx,
        .cols  = num_stages * nx,
    }};
    matrix invQᵀ{{
        .depth = vl * p,
        .rows  = nx,
        .cols  = num_stages * nx,
    }};
    matrix LBA{{
        .depth = vl * p,
        .rows  = nu + nx,
        .cols  = (num_stages - 1) * nx,
    }};
    for (index_t ti = 0; ti < num_proc; ++ti) {
        const index_t di0 = ti * num_stages; // data batch index
        for (index_t i = 0; i < num_stages; ++i) {
            const auto di = di0 + i;
            auto RSQ      = riccati_R̂ŜQ̂.batch(ti);
            auto RSQi     = RSQ.middle_cols(i * nux, nux);
            auto Qi       = tril(RSQi.bottom_right(nx, nx));
            auto Qi_inv   = triu(invQᵀ.batch(ti).middle_cols(i * nx, nx));
            auto Â        = riccati_ÂB̂.batch(ti).left_cols(num_stages * nx);
            auto Âi       = Â.middle_cols(i * nx, nx);
            auto AiQᵀ     = AinvQᵀ.batch(ti);
            auto AiQiᵀ    = AiQᵀ.middle_cols(i * nx, nx);
            auto BAᵀ      = riccati_BAᵀ.batch(ti);
            auto LBAt     = LBA.batch(ti);

            copy(Âi, AiQiᵀ);
            if (i + 1 < num_stages) // Final block already inverted
                trtri(Qi, Qi_inv.transposed());
            else
                copy(triu(RSQi.block(nu - 1, nu, nx, nx)), Qi_inv);
            if (i + 1 < num_stages) // Final block is already Â LQ⁻ᵀ
                trsm(AiQiᵀ, Qi.transposed());
            if (i > 0) {
                auto LBAi = LBAt.middle_cols((i - 1) * nx, nx);
                if (alt) {
                    auto RSQ_prev = RSQ.middle_cols((i - 1) * nux, nux);
                    auto Q_prev   = tril(RSQ_prev.bottom_right(nx, nx));
                    auto BA       = data_BA.batch(di);
                    copy(BA.transposed(), LBAi);
                    trmm(LBAi, Q_prev);
                } else {
                    auto BAᵀi = BAᵀ.middle_cols((i - 1) * nx, nx);
                    copy(BAᵀi, LBAi);
                }
            }
        }
    }
    for (index_t vi = 0; vi < vl; ++vi) {
        const index_t sv = vi * num_proc * (nuxx * num_stages - nx);
        for (index_t ti = 0; ti < num_proc; ++ti) {
            const index_t k0 = ti * num_stages + vi * vstride;
            const auto biA   = ti + vi * num_proc;
            const auto biI   = sub_wrap_P(biA, 1);
            const auto sλA   = sλ + nx * get_linear_batch_offset(biA);
            const auto sλI   = sλ + nx * get_linear_batch_offset(biI);
            auto B̂           = riccati_ÂB̂.batch(ti).right_cols(num_stages * nu);
            auto R̂ŜQ̂         = riccati_R̂ŜQ̂.batch(ti);
            auto LBAt        = LBA.batch(ti);
            // TODO: handle case if lev > or >= lp()
            for (index_t i = 0; i < num_stages; ++i) {
                [[maybe_unused]] const index_t k = sub_wrap_N(k0, i);
                index_t s                        = sv + ti * (nuxx * num_stages - nx) + nuxx * i;
                auto B̂i                          = B̂.middle_cols(i * nu, nu);
                auto R̂ŜQ̂i                        = R̂ŜQ̂.middle_cols(i * nux, nux);
                auto RSi                         = R̂ŜQ̂i(vi).left_cols(nu);
                auto Qi                          = R̂ŜQ̂i(vi).bottom_right(nx, nx);
                auto iQiᵀ                        = invQᵀ.batch(ti).middle_cols(i * nx, nx);
                auto AiQᵀ                        = AinvQᵀ.batch(ti);
                auto AiQiᵀ                       = AiQᵀ.middle_cols(i * nx, nx);
                if (i > 0) {
                    auto LBAi      = LBAt.middle_cols((i - 1) * nx, nx);
                    auto R̂ŜQ̂i_prev = R̂ŜQ̂.middle_cols((i - 1) * nux, nux);
                    auto iQᵀprev   = R̂ŜQ̂i_prev(vi).block(nu - 1, nu, nx, nx);
                    auto AiQprevᵀ  = AiQᵀ.middle_cols((i - 1) * nx, nx);
                    for (index_t c = 0; c < nx; ++c) {
                        for (index_t r = 0; r <= c; ++r)
                            mat.add(s - nx + r, s - nx + c, -iQᵀprev(r, c));
                        for (index_t r = 0; r < nux; ++r)
                            mat.add(s + r, s - nx + c, LBAi(vi)(r, c));
                        for (index_t r = 0; r < nx; ++r)
                            mat.add(sλA + r, s - nx + c, AiQprevᵀ(vi)(r, c));
                    }
                }
                for (index_t c = 0; c < nu; ++c) {
                    for (index_t r = c; r < nux; ++r)
                        mat.add(s + r, s + c, RSi(r, c));
                    for (index_t r = 0; r < nx; ++r)
                        mat.add(sλA + r, s + c, B̂i(vi)(r, c));
                }
                for (index_t c = 0; c < nx; ++c) {
                    for (index_t r = c; r < nx; ++r) {
                        mat.add(s + r + nu, s + c + nu, Qi(r, c));
                    }
                    if (i + 1 < num_stages)
                        for (index_t r = 0; r <= c; ++r)
                            mat.add(s + r + nux, s + c + nu, -iQiᵀ(vi)(r, c));
                    else
                        for (index_t r = 0; r <= c; ++r)
                            mat.add(sλI + r, s + c + nu, -iQiᵀ(vi)(r, c));
                    for (index_t r = 0; r < nx; ++r)
                        mat.add(sλA + r, s + c + nu, AiQiᵀ(vi)(r, c));
                }
            }
        }
    }
    index_t s               = sλ;
    const auto cyclic_block = [&](index_t i, index_t offset) {
        const index_t sY = sλ + nx * get_linear_batch_offset(i + offset);
        const index_t sU = sλ + nx * get_linear_batch_offset(i - offset);
        const index_t bi = i % p;
        const index_t vi = i / p;
        for (index_t c = 0; c < nx; ++c) {
            for (index_t r = c; r < nx; ++r)
                mat.add(s + r, s + c, cr_L.batch(bi)(vi)(r, c));
            if (i + offset < vl * p)
                for (index_t r = 0; r < nx; ++r)
                    mat.add(sY + r, s + c, cr_Y.batch(bi)(vi)(r, c));
            for (index_t r = 0; r < nx; ++r)
                mat.add(sU + r, s + c, cr_U.batch(bi)(vi)(r, c));
        }
        s += nx;
    };
    const auto cyclic_block_final = [&](index_t i, index_t offset) {
        const index_t sY = sλ + nx * get_linear_batch_offset(i + offset);
        const index_t bi = i % p;
        const index_t vi = i / p;
        for (index_t c = 0; c < nx; ++c) {
            for (index_t r = c; r < nx; ++r)
                mat.add(s + r, s + c, pcr_L.batch(0)(vi)(r, c));
            if (i + offset < vl * p)
                for (index_t r = 0; r < nx; ++r)
                    mat.add(sY + r, s + c, cr_Y.batch(bi)(vi)(r, c));
        }
        s += nx;
    };
    if (lP != lvl) {
        for (index_t i = 0; i < ((p * vl) >> 1); ++i)
            cyclic_block(2 * i + 1, 1);
        for (index_t l = 1; l < lp(); ++l) {
            index_t offset = 1 << l;
            index_t stride = offset << 1;
            for (index_t i = offset; i < vl * p; i += stride)
                cyclic_block(i, offset);
        }
    }
    for (index_t i = 0; i < vl * p; i += p) {
        cyclic_block_final(i, p);
    }
    return std::move(mat).build();
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::build_sparse_diag() const -> SparseMatrix {
    const index_t nux = nu + nx, nuxx = nux + nx;
    const index_t num_stages = n; // number of stages per thread
    const index_t num_proc   = p;
    const index_t nn         = ceil_N() * nuxx;
    const index_t sλ         = nn - (nx * p * vl);
    SparseMatrixBuilder mat{.rows = nn, .cols = nn, .symmetry = Symmetry::Lower};
    for (index_t vi = 0; vi < vl; ++vi) {
        const index_t sv = vi * num_proc * (nuxx * num_stages - nx);
        for (index_t ti = 0; ti < num_proc; ++ti) {
            for (index_t i = 0; i < num_stages; ++i) {
                index_t s = sv + ti * (nuxx * num_stages - nx) + nuxx * i;
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
    for (index_t i = 0; i < p * vl; ++i)
        for (index_t r = 0; r < nx; ++r)
            mat.add(sλ + nx * i + r, sλ + nx * i + r, -1);
    return std::move(mat).build();
}

} // namespace CYQLONE_NS(cyqlone)::v2
