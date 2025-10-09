#include <cyqlone/cyqlone.hpp>

#include <batmat/assume.hpp>
#include <batmat/linalg/simdify.hpp>
#include <guanaqo/blas/hl-blas-interface.hpp>
#include <cmath>
#include <limits>
#if !BATMAT_WITH_OPENMP
#include <batmat/thread-pool.hpp>
#endif

namespace cyqlone {
using batmat::linalg::simdify;

template <index_t VL, class T, StorageOrder DefaultOrder>
CyqloneSolver<VL, T, DefaultOrder>
CyqloneSolver<VL, T, DefaultOrder>::build(const CyqloneStorage<value_type> &ocp, index_t lP) {
#if !BATMAT_WITH_OPENMP
    const auto n_thr = size_t{1} << (lP - lvl);
    if (!batmat::pool || batmat::pool->size() < n_thr)
        batmat::pool.emplace(n_thr);
#endif
    CyqloneSolver<VL, T, DefaultOrder> res{
        .N_horiz = ocp.N_horiz,
        .nx      = ocp.nx,
        .nu      = ocp.nu,
        .ny      = ocp.ny,
        .ny_0    = ocp.ny_0,
        .ny_N    = ocp.ny_N,
        .lP      = lP,
    };
    res.update_data(ocp);
    return res;
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::update_data(const CyqloneStorage<value_type> &ocp) {
    BATMAT_ASSERT(ocp.N_horiz == N_horiz);
    BATMAT_ASSERT(ocp.nx == nx);
    BATMAT_ASSERT(ocp.nu == nu);
    BATMAT_ASSERT(ocp.ny == ny);
    BATMAT_ASSERT(ocp.ny_0 == ny_0);
    BATMAT_ASSERT(ocp.ny_N == ny_N);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k = sub_wrap_N(k0 + vi * vstride, i);
                if (k < N_horiz) {
                    detail::copy(ocp.data_F(k), data_BA.batch(di)(vi));
                    detail::copy(ocp.data_H(k), data_RSQ.batch(di)(vi));
                    if (k == 0)
                        detail::copy(ocp.data_G0N(0).transposed(),
                                     data_DCᵀ.batch(di)(vi).left_cols(ny_0 + ny_N));
                    else
                        detail::copy(ocp.data_G(k - 1).transposed(),
                                     data_DCᵀ.batch(di)(vi).left_cols(ny));
                } else {
                    using std::pow;
                    const auto ε = pow(std::numeric_limits<value_type>::min(), value_type(0.25));
                    data_RSQ.batch(di)(vi).top_left(nu, nu).add_to_diagonal(1);
                    data_RSQ.batch(di)(vi).bottom_right(nx, nx).add_to_diagonal(ε);
                    data_BA.batch(di)(vi).right_cols(nx).add_to_diagonal(1);
                }
            }
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::initialize_rhs(const CyqloneStorage<value_type> &ocp,
                                                        mut_view<> rhs) const {
    BATMAT_ASSERT(rhs.depth() == ceil_N);
    BATMAT_ASSERT(rhs.rows() == nx);
    BATMAT_ASSERT(rhs.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k = sub_wrap_N(k0 + vi * vstride, i);
                if (k < N_horiz) {
                    rhs.batch(di)(vi) = ocp.data_c(k);
                } else {
                    rhs.batch(di)(vi).set_constant(0);
                }
            }
            compact_blas::xneg(simdify(rhs.batch(di))); // TODO
        }
    }
}

template <index_t VL, class T, StorageOrder DefaultOrder>
void CyqloneSolver<VL, T, DefaultOrder>::initialize_gradient(const CyqloneStorage<value_type> &ocp,
                                                             mut_view<> grad) const {
    BATMAT_ASSERT(grad.depth() == ceil_N);
    BATMAT_ASSERT(grad.rows() == nu + nx);
    BATMAT_ASSERT(grad.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k = sub_wrap_N(k0 + vi * vstride, i);
                if (k < N_horiz) {
                    grad.batch(di)(vi) = ocp.data_rq(k);
                } else {
                    grad.batch(di)(vi).set_constant(0);
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
    BATMAT_ASSERT(b_min.depth() == ceil_N);
    BATMAT_ASSERT(b_min.rows() == nyM);
    BATMAT_ASSERT(b_min.cols() == 1);
    BATMAT_ASSERT(b_max.depth() == ceil_N);
    BATMAT_ASSERT(b_max.rows() == nyM);
    BATMAT_ASSERT(b_max.cols() == 1);
    const auto inf           = std::numeric_limits<value_type>::infinity();
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * vstride, i);
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
    BATMAT_ASSERT(ux.depth() == ceil_N);
    BATMAT_ASSERT(ux.rows() == nux);
    BATMAT_ASSERT(ux.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * vstride, i);
                using crview = guanaqo::MatrixView<const value_type, index_t>;
                if (k == 0) {
                    ux.batch(di)(vi).top_rows(nu) = crview::as_column(ux_lin.first(nu));
                    ux.batch(di)(vi).bottom_rows(nx) =
                        crview::as_column(ux_lin.subspan(nu + (N_horiz - 1) * nux, nx));
                } else if (k < N_horiz) {
                    ux.batch(di)(vi).top_rows(nu) = crview::as_column(ux_lin.subspan(k * nux, nu));
                    ux.batch(di)(vi).bottom_rows(nx) =
                        crview::as_column(ux_lin.subspan(nu + (k - 1) * nux, nx));
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
    BATMAT_ASSERT(ux.depth() == ceil_N);
    BATMAT_ASSERT(ux.rows() == nux);
    BATMAT_ASSERT(ux.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k      = sub_wrap_N(k0 + vi * vstride, i);
                using rview = guanaqo::MatrixView<value_type, index_t>;
                if (k == 0) {
                    rview::as_column(ux_lin.first(nu)) = ux.batch(di)(vi).top_rows(nu);
                    rview::as_column(ux_lin.subspan(nu + (N_horiz - 1) * nux, nx)) =
                        ux.batch(di)(vi).bottom_rows(nx);
                } else if (k < N_horiz) {
                    rview::as_column(ux_lin.subspan(k * nux, nu)) = ux.batch(di)(vi).top_rows(nu);
                    rview::as_column(ux_lin.subspan(nu + (k - 1) * nux, nx)) =
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
    BATMAT_ASSERT(λ.depth() == ceil_N);
    BATMAT_ASSERT(λ.rows() == nλ);
    BATMAT_ASSERT(λ.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * vstride, i);
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
    BATMAT_ASSERT(λ.depth() == ceil_N);
    BATMAT_ASSERT(λ.rows() == nλ);
    BATMAT_ASSERT(λ.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k      = sub_wrap_N(k0 + vi * vstride, i);
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
    BATMAT_ASSERT(y.depth() == ceil_N);
    BATMAT_ASSERT(y.rows() == std::max(ny, ny_0 + ny_N));
    BATMAT_ASSERT(y.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k       = sub_wrap_N(k0 + vi * vstride, i);
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
    BATMAT_ASSERT(y.depth() == ceil_N);
    BATMAT_ASSERT(y.rows() == std::max(ny, ny_0 + ny_N));
    BATMAT_ASSERT(y.cols() == 1);
    const auto vstride       = ceil_N >> lvl;
    const index_t num_stages = ceil_N >> lP; // number of stages per thread
    for (index_t ti = 0; ti < (1 << (lP - lvl)); ++ti) {
        const index_t k0  = ti * num_stages;
        const index_t di0 = ti * num_stages;
        for (index_t i = 0; i < num_stages; ++i) {
            index_t di = di0 + i;
            for (index_t vi = 0; vi < vl; ++vi) {
                auto k      = sub_wrap_N(k0 + vi * vstride, i);
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

} // namespace cyqlone
