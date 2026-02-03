#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/simdify.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <limits>
#if !BATMAT_WITH_OPENMP
#include <batmat/thread-pool.hpp>
#endif

namespace CYQLONE_NS(cyqlone) {

using batmat::linalg::simdify;

namespace detail {
/// Simple (inefficient) matrix copy that supports slices with non-unit strides.
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
/// Simple (inefficient) scaled matrix copy that supports slices with non-unit strides.
void scale(T0 scalar, guanaqo::MatrixView<T1, I1, S1, O1> src,
           guanaqo::MatrixView<T2, I2, S2, O2> dst) {
    assert(src.rows == dst.rows);
    assert(src.cols == dst.cols);
    for (index_t r = 0; r < src.rows; ++r) // TODO: optimize
        for (index_t c = 0; c < src.cols; ++c)
            dst(r, c) = scalar * src(r, c);
}
} // namespace detail

template <index_t VL, class T, StorageOrder DefaultOrder>
CyqloneSolver<VL, T, DefaultOrder>
CyqloneSolver<VL, T, DefaultOrder>::build(const CyqloneStorage<value_type> &ocp, index_t p) {
    BATMAT_ASSERT(p > 0);
    BATMAT_ASSERT(v == 1 || is_pow_2(p));
    CyqloneSolver<VL, T, DefaultOrder> res{
        .N_horiz = ocp.N_horiz,
        .nx      = ocp.nx,
        .nu      = ocp.nu,
        .ny      = ocp.ny,
        .ny_0    = ocp.ny_0,
        .ny_N    = ocp.ny_N,
        .p       = p,
    };
    res.update_data(ocp);
    return res;
}

// For lgp = 5, lgv = 2, N = 3 << lgp
//
// | Stage j | Thread c | Index i | Data di | λ(A) | λ(I) | bλ(A) | bλ(I) |
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

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_data(const CyqloneStorage<value_type> &ocp) {
    BATMAT_ASSERT(ocp.N_horiz == N_horiz);
    BATMAT_ASSERT(ocp.nx == nx);
    BATMAT_ASSERT(ocp.nu == nu);
    BATMAT_ASSERT(ocp.ny == ny);
    BATMAT_ASSERT(ocp.ny_0 == ny_0);
    BATMAT_ASSERT(ocp.ny_N == ny_N);
    const auto scale_QN = 1 / static_cast<value_type>(ceil_N() - N_horiz + 1);
    for (index_t c = 0; c < p; ++c) {
        const index_t k0  = c * n;
        const index_t di0 = c * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k = sub_wrap_N(k0 + vi * p * n, i);
                if (k == 0) {
                    if (ceil_N() == N_horiz) {
                        detail::copy(ocp.data_F(0), data_F.batch(di)(vi)); // A, B
                        detail::copy(ocp.data_H(0), data_H.batch(di)(vi)); // R, S, Q
                    } else {
                        detail::copy(ocp.data_F(0).left_cols(nu),
                                     data_F.batch(di)(vi).left_cols(nu));           // B
                        data_F.batch(di)(vi).right_cols(nx).set_constant(0);        // A = 0
                        detail::copy(ocp.data_H(0).top_left(nu, nu),                //
                                     data_H.batch(di)(vi).top_left(nu, nu));        // R
                        data_H.batch(di)(vi).bottom_left(nx, nu).set_constant(0);   // S = 0
                        detail::scale(scale_QN, ocp.data_H(0).bottom_right(nx, nx), //
                                      data_H.batch(di)(vi).bottom_right(nx, nx));   // Q = α Q(N)
                    }
                    detail::copy(ocp.data_G0N(0).transposed(),
                                 data_Gᵀ.batch(di)(vi).left_cols(ny_0 + ny_N)); // D, C
                } else if (k < N_horiz) {
                    detail::copy(ocp.data_F(k), data_F.batch(di)(vi)); // A, B
                    detail::copy(ocp.data_H(k), data_H.batch(di)(vi)); // R, S, Q
                    detail::copy(ocp.data_G(k - 1).transposed(),       //
                                 data_Gᵀ.batch(di)(vi).left_cols(ny)); // D, C
                } else {
                    data_F.batch(di)(vi).set_constant(0);                       // B = 0
                    data_F.batch(di)(vi).right_cols(nx).set_diagonal(1);        // A = I
                    data_H.batch(di)(vi).left_cols(nu).set_constant(0);         // S = 0
                    data_H.batch(di)(vi).top_left(nu, nu).set_diagonal(1);      // R = I
                    detail::scale(scale_QN, ocp.data_H(0).bottom_right(nx, nx), //
                                  data_H.batch(di)(vi).bottom_right(nx, nx));   // Q = α Q(N)
                    data_Gᵀ.batch(di)(vi).left_cols(ny).set_constant(0);        // D, C
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::initialize_rhs(const CyqloneStorage<value_type> &ocp,
                                                        mut_view<> rhs) const {
    BATMAT_ASSERT(rhs.depth() == ceil_N());
    BATMAT_ASSERT(rhs.rows() == nx);
    BATMAT_ASSERT(rhs.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k = sub_wrap_N(k0 + vi * p * n, i);
                if (k < N_horiz) {
                    rhs.batch(di)(vi) = ocp.data_c(k);
                } else {
                    rhs.batch(di)(vi).set_constant(0);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::initialize_gradient(const CyqloneStorage<value_type> &ocp,
                                                             mut_view<> grad) const {
    BATMAT_ASSERT(grad.depth() == ceil_N());
    BATMAT_ASSERT(grad.rows() == nu + nx);
    BATMAT_ASSERT(grad.cols() == 1);
    const auto scale_qN = 1 / static_cast<value_type>(ceil_N() - N_horiz + 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k = sub_wrap_N(k0 + vi * p * n, i);
                if (k == 0) {
                    if (ceil_N() == N_horiz) {
                        grad.batch(di)(vi) = ocp.data_rq(0);
                    } else {
                        grad.batch(di)(vi).top_rows(nu) = ocp.data_rq(0).top_rows(nu);
                        detail::scale(scale_qN, ocp.data_rq(0).bottom_rows(nx),
                                      grad.batch(di)(vi).bottom_rows(nx));
                    }
                } else if (k < N_horiz) {
                    grad.batch(di)(vi) = ocp.data_rq(k);
                } else {
                    grad.batch(di)(vi).top_rows(nu).set_constant(0);
                    detail::scale(scale_qN, ocp.data_rq(0).bottom_rows(nx),
                                  grad.batch(di)(vi).bottom_rows(nx));
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::initialize_bounds(const CyqloneStorage<value_type> &ocp,
                                                           mut_view<> b_min,
                                                           mut_view<> b_max) const {
    const index_t nyM = std::max(ny, ny_0 + ny_N);
    BATMAT_ASSERT(b_min.depth() == ceil_N());
    BATMAT_ASSERT(b_min.rows() == nyM);
    BATMAT_ASSERT(b_min.cols() == 1);
    BATMAT_ASSERT(b_max.depth() == ceil_N());
    BATMAT_ASSERT(b_max.rows() == nyM);
    BATMAT_ASSERT(b_max.cols() == 1);
    const auto inf = std::numeric_limits<value_type>::infinity();
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * p * n, i);
                auto b_min_i = b_min.batch(di)(vi), b_max_i = b_max.batch(di)(vi);
                if (k == 0) {
                    b_min_i.top_rows(ny_0 + ny_N) = ocp.data_lb0N(0);
                    b_max_i.top_rows(ny_0 + ny_N) = ocp.data_ub0N(0);
                    b_min_i.bottom_rows(nyM - ny_0 - ny_N).set_constant(-inf);
                    b_max_i.bottom_rows(nyM - ny_0 - ny_N).set_constant(+inf);
                } else if (k < N_horiz) {
                    b_min_i.top_rows(ny) = ocp.data_lb(k - 1);
                    b_max_i.top_rows(ny) = ocp.data_ub(k - 1);
                    b_min_i.bottom_rows(nyM - ny).set_constant(-inf);
                    b_max_i.bottom_rows(nyM - ny).set_constant(+inf);
                } else {
                    b_min_i.set_constant(-inf);
                    b_max_i.set_constant(+inf);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::pack_variables(std::span<const value_type> ux_lin,
                                                        mut_view<> ux) const {
    const index_t nux = nu + nx;
    BATMAT_ASSERT(static_cast<index_t>(ux_lin.size()) == nux * N_horiz);
    BATMAT_ASSERT(ux.depth() == ceil_N());
    BATMAT_ASSERT(ux.rows() == nux);
    BATMAT_ASSERT(ux.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * p * n, i);
                using crview = guanaqo::MatrixView<const value_type, index_t>;
                if (k == 0) {
                    ux.batch(di)(vi).top_rows(nu) = crview::as_column(ux_lin.first(nu));
                    if (ceil_N() == N_horiz)
                        ux.batch(di)(vi).bottom_rows(nx) =
                            crview::as_column(ux_lin.subspan(nu + (N_horiz - 1) * nux, nx));
                    else
                        ux.batch(di)(vi).bottom_rows(nx).set_constant(0);
                } else if (k < N_horiz) {
                    ux.batch(di)(vi).top_rows(nu) = crview::as_column(ux_lin.subspan(k * nux, nu));
                    ux.batch(di)(vi).bottom_rows(nx) =
                        crview::as_column(ux_lin.subspan(nu + (k - 1) * nux, nx));
                } else if (k == N_horiz) {
                    // only pack the last state if we have padding stages
                    ux.batch(di)(vi).bottom_rows(nx) =
                        crview::as_column(ux_lin.subspan(nu + (N_horiz - 1) * nux, nx));
                } else {
                    ux.batch(di)(vi).set_constant(0);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::unpack_variables(view<> ux,
                                                          std::span<value_type> ux_lin) const {
    const index_t nux = nu + nx;
    BATMAT_ASSERT(static_cast<index_t>(ux_lin.size()) == nux * N_horiz);
    BATMAT_ASSERT(ux.depth() == ceil_N());
    BATMAT_ASSERT(ux.rows() == nux);
    BATMAT_ASSERT(ux.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k      = sub_wrap_N(k0 + vi * p * n, i);
                using rview = guanaqo::MatrixView<value_type, index_t>;
                if (k == 0) {
                    rview::as_column(ux_lin.first(nu)) = ux.batch(di)(vi).top_rows(nu);
                    if (ceil_N() == N_horiz)
                        rview::as_column(ux_lin.subspan(nu + (N_horiz - 1) * nux, nx)) =
                            ux.batch(di)(vi).bottom_rows(nx);
                } else if (k < N_horiz) {
                    rview::as_column(ux_lin.subspan(k * nux, nu)) = ux.batch(di)(vi).top_rows(nu);
                    rview::as_column(ux_lin.subspan(nu + (k - 1) * nux, nx)) =
                        ux.batch(di)(vi).bottom_rows(nx);
                } else if (k == N_horiz) {
                    // only unpack the last state if we have padding stages
                    rview::as_column(ux_lin.subspan(nu + (N_horiz - 1) * nux, nx)) =
                        ux.batch(di)(vi).bottom_rows(nx);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::pack_dynamics(std::span<const value_type> λ_lin,
                                                       mut_view<> λ) const {
    const index_t nλ = nx;
    BATMAT_ASSERT(static_cast<index_t>(λ_lin.size()) == nλ * N_horiz);
    BATMAT_ASSERT(λ.depth() == ceil_N());
    BATMAT_ASSERT(λ.rows() == nλ);
    BATMAT_ASSERT(λ.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * p * n, i);
                using crview = guanaqo::MatrixView<const value_type, index_t>;
                if (k < N_horiz) {
                    λ.batch(di)(vi) = crview::as_column(λ_lin.subspan(k * nλ, nλ));
                } else {
                    λ.batch(di)(vi).set_constant(0);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::unpack_dynamics(view<> λ,
                                                         std::span<value_type> λ_lin) const {
    const index_t nλ = nx;
    BATMAT_ASSERT(static_cast<index_t>(λ_lin.size()) == nλ * N_horiz);
    BATMAT_ASSERT(λ.depth() == ceil_N());
    BATMAT_ASSERT(λ.rows() == nλ);
    BATMAT_ASSERT(λ.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k      = sub_wrap_N(k0 + vi * p * n, i);
                using rview = guanaqo::MatrixView<value_type, index_t>;
                if (k < N_horiz) {
                    rview::as_column(λ_lin.subspan(k * nλ, nλ)) = λ.batch(di)(vi);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::pack_constraints(std::span<const value_type> y_lin,
                                                          mut_view<> y, value_type fill) const {
    BATMAT_ASSERT(static_cast<index_t>(y_lin.size()) == ny * (N_horiz - 1) + ny_0 + ny_N);
    BATMAT_ASSERT(y.depth() == ceil_N());
    BATMAT_ASSERT(y.rows() == std::max(ny, ny_0 + ny_N));
    BATMAT_ASSERT(y.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * p * n, i);
                using crview = guanaqo::MatrixView<const value_type, index_t>;
                if (k == 0) {
                    index_t ny_pad                 = std::max(ny, ny_0 + ny_N) - (ny_0 + ny_N);
                    y.batch(di)(vi).top_rows(ny_0) = crview::as_column(y_lin.first(ny_0));
                    y.batch(di)(vi).bottom_rows(ny_N) =
                        crview::as_column(y_lin.subspan(ny_0 + (N_horiz - 1) * ny, ny_N));
                    y.batch(di)(vi).bottom_rows(ny_pad).set_constant(fill);
                } else if (k < N_horiz) {
                    index_t ny_pad = std::max(ny, ny_0 + ny_N) - ny;
                    y.batch(di)(vi).top_rows(ny) =
                        crview::as_column(y_lin.subspan(ny_0 + (k - 1) * ny, ny));
                    y.batch(di)(vi).bottom_rows(ny_pad).set_constant(fill);
                } else {
                    y.batch(di)(vi).set_constant(0);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::unpack_constraints(view<> y,
                                                            std::span<value_type> y_lin) const {
    BATMAT_ASSERT(static_cast<index_t>(y_lin.size()) == ny * (N_horiz - 1) + ny_0 + ny_N);
    BATMAT_ASSERT(y.depth() == ceil_N());
    BATMAT_ASSERT(y.rows() == std::max(ny, ny_0 + ny_N));
    BATMAT_ASSERT(y.cols() == 1);
    for (index_t ti = 0; ti < p; ++ti) {
        const index_t k0  = ti * n;
        const index_t di0 = ti * n;
        for (index_t i = 0; i < n; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < v; ++vi) {
                auto k      = sub_wrap_N(k0 + vi * p * n, i);
                using rview = guanaqo::MatrixView<value_type, index_t>;
                if (k == 0) {
                    rview::as_column(y_lin.first(ny_0)) = y.batch(di)(vi).top_rows(ny_0);
                    rview::as_column(y_lin.subspan(ny_0 + (N_horiz - 1) * ny, ny_N)) =
                        y.batch(di)(vi).bottom_rows(ny_N);
                } else if (k < N_horiz) {
                    rview::as_column(y_lin.subspan(ny_0 + (k - 1) * ny, ny)) =
                        y.batch(di)(vi).top_rows(ny);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::initialize_rhs(const CyqloneStorage<value_type> &ocp) const
    -> matrix<> {
    matrix<> rhs = initialize_dynamics_constraints();
    initialize_rhs(ocp, rhs);
    return rhs;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::initialize_gradient(
    const CyqloneStorage<value_type> &ocp) const -> matrix<> {
    matrix<> grad = initialize_variables();
    initialize_gradient(ocp, grad);
    return grad;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::initialize_bounds(
    const CyqloneStorage<value_type> &ocp) const -> std::pair<matrix<>, matrix<>> {
    std::pair b{initialize_general_constraints(), initialize_general_constraints()};
    initialize_bounds(ocp, b.first, b.second);
    return b;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::pack_variables(std::span<const value_type> ux_lin) const
    -> matrix<> {
    matrix<> ux = initialize_variables();
    pack_variables(ux_lin, ux);
    return ux;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::unpack_variables(view<> ux) const
    -> std::vector<value_type> {
    std::vector<value_type> ux_lin(num_variables());
    unpack_variables(ux, ux_lin);
    return ux_lin;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::pack_dynamics(std::span<const value_type> λ_lin) const
    -> matrix<> {
    matrix<> λ = initialize_dynamics_constraints();
    pack_dynamics(λ_lin, λ);
    return λ;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::unpack_dynamics(view<> λ) const
    -> std::vector<value_type> {
    std::vector<value_type> λ_lin(num_dynamics_constraints());
    unpack_dynamics(λ, λ_lin);
    return λ_lin;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::pack_constraints(std::span<const value_type> y_lin,
                                                          value_type fill) const -> matrix<> {
    matrix<> y = initialize_general_constraints();
    pack_constraints(y_lin, y, fill);
    return y;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::unpack_constraints(view<> y) const
    -> std::vector<value_type> {
    std::vector<value_type> y_lin(num_general_constraints());
    unpack_constraints(y, y_lin);
    return y_lin;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::initialize_variables() const -> matrix<> {
    return matrix<>{{.depth = ceil_N(), .rows = nu + nx, .cols = 1}};
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::initialize_dynamics_constraints() const -> matrix<> {
    return matrix<>{{.depth = ceil_N(), .rows = nx, .cols = 1}};
}

template <index_t VL, class T, StorageOrder DefaultOrder>
auto CyqloneSolver<VL, T, DefaultOrder>::initialize_general_constraints() const -> matrix<> {
    return matrix<>{{.depth = ceil_N(), .rows = std::max(ny, ny_0 + ny_N), .cols = 1}};
}

} // namespace CYQLONE_NS(cyqlone)
