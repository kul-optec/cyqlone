#include <cyqlone/cyqlone-storage.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>

namespace cyqlone {

template <class T>
void CyqloneStorage<T>::reconstruct_ineq_multipliers(const LinearOCPStorage &ocp,
                                                     std::span<const value_type> y_compressed,
                                                     std::span<value_type> y) {
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    // Count the number of input constraints in the first stage
    std::vector<bool> Ju0(ny);
    for (index_t c = 0; c < nu; ++c)
        for (index_t r = 0; r < ny; ++r)
            if (ocp.D(0)(r, c) != 0)
                Ju0[r] = true;
    const auto ny_0 = static_cast<index_t>(std::ranges::count(Ju0, true));
    BATMAT_ASSERT(static_cast<index_t>(y.size()) == N * ny + ny_N);
    BATMAT_ASSERT(static_cast<index_t>(y_compressed.size()) == (N - 1) * ny + ny_0 + ny_N);
    for (index_t r = 0, j = 0; r < ny; ++r)
        if (Ju0[r])
            y[r] = y_compressed[j++];
        else
            y[r] = 0;
    std::ranges::copy(y_compressed.subspan(ny_0), y.begin() + ny);
}

template <class T>
CyqloneStorage<T>
CyqloneStorage<T>::build(const LinearOCPStorage &ocp, std::span<const value_type> qr,
                         std::span<const value_type> b_eq, std::span<const value_type> b_lb,
                         std::span<const value_type> b_ub) {
    using vw                         = guanaqo::MatrixView<const value_type, index_t>;
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    const auto nux                   = nu + nx;
    // Count the number of input constraints in the first stage
    std::vector<bool> Ju0(ny);
    for (index_t c = 0; c < nu; ++c)
        for (index_t r = 0; r < ny; ++r)
            if (ocp.D(0)(r, c) != 0)
                Ju0[r] = true;
    const auto ny_0 = static_cast<index_t>(std::ranges::count(Ju0, true));
    CyqloneStorage<T> res{.N_horiz = N, .nx = nx, .nu = nu, .ny = ny, .ny_0 = ny_0, .ny_N = ny_N};
    // H₀ = [ R₀ 0 ]
    //      [ 0  Qₙ]
    res.data_H(0).top_left(nu, nu)     = ocp.R(0);
    res.data_H(0).bottom_right(nx, nx) = ocp.Q(N);
    res.data_H(0).bottom_left(nx, nu).set_constant(0);
    res.data_H(0).top_right(nu, nx).set_constant(0);
    // F₀ = [ B₀ 0 ]
    res.data_F(0).left_cols(nu) = ocp.B(0);
    res.data_F(0).right_cols(nx).set_constant(0);
    // G₀ = [ D₀ 0 ]
    //      [ 0  Cₙ]
    res.data_G(0).bottom_left(ny_N, nu).set_constant(0);
    res.data_G(0).top_right(ny_0, nx).set_constant(0);
    for (index_t r = 0, j = 0; r < ny; ++r) {
        if (Ju0[r]) {
            res.data_G0N(0).block(j, 0, 1, nu) = ocp.D(0).middle_rows(r, 1);
            value_type t                       = 0;
            for (index_t c = 0; c < nx; ++c) // lb - C₀ x₀
                t += ocp.C(0)(r, c) * b_eq[c];
            res.data_lb0N(0, j, 0) = b_lb[r] - t;
            res.data_ub0N(0, j, 0) = b_ub[r] - t;
            res.indices_G0[j]      = r;
            ++j;
        }
    }
    res.data_G0N(0).bottom_right(ny_N, nx) = ocp.C(N);
    res.data_lb0N(0).bottom_rows(ny_N)     = vw::as_column(b_lb.subspan(N * ny, ny_N));
    res.data_ub0N(0).bottom_rows(ny_N)     = vw::as_column(b_ub.subspan(N * ny, ny_N));
    // c̃₀ = c₀ + A₀ x₀      (b_eq = [x₀, c₀, ... cₙ₋₁])
    res.data_c(0) = vw::as_column(b_eq.subspan(nx, nx));
    for (index_t r = 0; r < nx; ++r)
        for (index_t c = 0; c < nx; ++c)
            res.data_c(0, r, 0) += ocp.A(0)(r, c) * b_eq[c];
    // r̃₀ = r₀ + S₀ x₀
    res.data_rq(0).bottom_rows(nx) = vw::as_column(qr.subspan(nux * N, nx));
    res.data_rq(0).top_rows(nu)    = vw::as_column(qr.subspan(nx, nu));
    for (index_t r = 0; r < nu; ++r)
        for (index_t c = 0; c < nx; ++c)
            res.data_rq(0, r, 0) += ocp.S_trans(0)(c, r) * b_eq[c];
    for (index_t i = 1; i < N; ++i) {
        res.data_H(i).top_left(nu, nu)     = ocp.R(i);
        res.data_H(i).bottom_left(nx, nu)  = ocp.S_trans(i);
        res.data_H(i).top_right(nu, nx)    = ocp.S(i);
        res.data_H(i).bottom_right(nx, nx) = ocp.Q(i);
        res.data_F(i).left_cols(nu)        = ocp.B(i);
        res.data_F(i).right_cols(nx)       = ocp.A(i);
        res.data_G(i - 1).left_cols(nu)    = ocp.D(i);
        res.data_G(i - 1).right_cols(nx)   = ocp.C(i);
        res.data_lb(i - 1)                 = vw::as_column(b_lb.subspan(i * ny, ny));
        res.data_ub(i - 1)                 = vw::as_column(b_ub.subspan(i * ny, ny));
        res.data_c(i)                      = vw::as_column(b_eq.subspan((i + 1) * nx, nx));
        res.data_rq(i).bottom_rows(nx)     = vw::as_column(qr.subspan(i * nux, nx));
        res.data_rq(i).top_rows(nu)        = vw::as_column(qr.subspan(i * nux + nx, nu));
    }
    return res;
}

template struct CyqloneStorage<double>;
#if BATMAT_WITH_SINGLE
template struct CyqloneStorage<float>;
#endif

} // namespace cyqlone
