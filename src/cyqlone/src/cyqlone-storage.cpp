#include <cyqlone/cyqlone-storage.hpp>
#include <batmat/assume.hpp>
#include <batmat/config.hpp>

namespace cyqlone {

// TODO: make member function to reuse Ju0 and ny_0?
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
index_t CyqloneStorage<T>::count_constr_0(const LinearOCPStorage &ocp, std::vector<bool> &Ju0) {
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    const auto nJu0                  = static_cast<index_t>(Ju0.size());
    BATMAT_ASSUME(nJu0 == ny);
    for (index_t c = 0; c < nu; ++c)
        for (index_t r = 0; r < ny; ++r)
            Ju0[r] = Ju0[r] || ocp.D(0)(r, c) != 0;
    return static_cast<index_t>(std::ranges::count(Ju0, true));
}

template <class T>
void CyqloneStorage<T>::update(const LinearOCPStorage &ocp) {
    const auto ny_0_ = count_constr_0(ocp, Ju0);
    BATMAT_ASSERT(ny_0_ <= ny_0);
    update_impl(ocp);
}

template <class T>
void CyqloneStorage<T>::update_impl(const LinearOCPStorage &ocp) {
    const auto N = N_horiz;
    // H₀ = [ R₀ 0 ]
    //      [ 0  Qₙ]
    data_H(0).top_left(nu, nu)     = ocp.R(0);
    data_H(0).bottom_right(nx, nx) = ocp.Q(N);
    data_H(0).bottom_left(nx, nu).set_constant(0);
    data_H(0).top_right(nu, nx).set_constant(0);
    // F₀ = [ B₀ 0 ]
    data_F(0).left_cols(nu) = ocp.B(0);
    data_F(0).right_cols(nx).set_constant(0);
    // G₀ = [ D₀ 0 ]  ny_0
    //      [ 0  Cₙ]  ny_N
    data_G(0).bottom_left(ny_N, nu).set_constant(0);
    data_G(0).top_right(ny_0, nx).set_constant(0);
    index_t j = 0;
    for (index_t r = 0; r < ny; ++r) {
        if (Ju0[r]) {
            BATMAT_ASSUME(j < ny_0);
            data_G0N(0).block(j, 0, 1, nu) = ocp.D(0).middle_rows(r, 1);
            value_type t                   = 0;
            for (index_t c = 0; c < nx; ++c) // lb - C₀ x₀
                t += ocp.C(0)(r, c) * ocp.b(0)(c, 0);
            data_lb0N(0, j, 0) = ocp.b_min(0)(r, 0) - t;
            data_ub0N(0, j, 0) = ocp.b_max(0)(r, 0) - t;
            indices_G0[j]      = r;
            ++j;
        }
    }
    data_G0N(0).block(j, 0, ny_0 - j, nu).set_constant(0);
    data_G0N(0).bottom_right(ny_N, nx) = ocp.C(N);
    data_lb0N(0).bottom_rows(ny_N)     = ocp.b_min().bottom_rows(ny_N);
    data_ub0N(0).bottom_rows(ny_N)     = ocp.b_max().bottom_rows(ny_N);
    // c̃₀ = c₀ + A₀ x₀      (b_eq = [x₀, c₀, ... cₙ₋₁])
    data_c(0) = ocp.b(1);
    for (index_t r = 0; r < nx; ++r)
        for (index_t c = 0; c < nx; ++c)
            data_c(0, r, 0) += ocp.A(0)(r, c) * ocp.b(0)(c, 0);
    // r̃₀ = r₀ + S₀ x₀
    data_rq(0).bottom_rows(nx) = ocp.q(N);
    data_rq(0).top_rows(nu)    = ocp.r(0);
    for (index_t r = 0; r < nu; ++r)
        for (index_t c = 0; c < nx; ++c)
            data_rq(0, r, 0) += ocp.S_trans(0)(c, r) * ocp.b(0)(c, 0);
    for (index_t i = 1; i < N; ++i) {
        data_H(i).top_left(nu, nu)     = ocp.R(i);
        data_H(i).bottom_left(nx, nu)  = ocp.S_trans(i);
        data_H(i).top_right(nu, nx)    = ocp.S(i);
        data_H(i).bottom_right(nx, nx) = ocp.Q(i);
        data_F(i).left_cols(nu)        = ocp.B(i);
        data_F(i).right_cols(nx)       = ocp.A(i);
        data_G(i - 1).left_cols(nu)    = ocp.D(i);
        data_G(i - 1).right_cols(nx)   = ocp.C(i);
        data_lb(i - 1)                 = ocp.b_min(i);
        data_ub(i - 1)                 = ocp.b_max(i);
        data_c(i)                      = ocp.b(i + 1);
        data_rq(i).bottom_rows(nx)     = ocp.q(i);
        data_rq(i).top_rows(nu)        = ocp.r(i);
    }
}

template <class T>
CyqloneStorage<T> CyqloneStorage<T>::build(const LinearOCPStorage &ocp, index_t ny_0) {
    const auto [N, nx, nu, ny, ny_N] = ocp.dim;
    // Count the number of input constraints in the first stage
    std::vector<bool> Ju0(ny);
    const auto ny_0_ = count_constr_0(ocp, Ju0);
    if (ny_0 < 0)
        ny_0 = ny_0_;
    else
        BATMAT_ASSERT(ny_0_ <= ny_0);
    CyqloneStorage<T> res{.N_horiz = N,
                          .nx      = nx,
                          .nu      = nu,
                          .ny      = ny,
                          .ny_0    = ny_0,
                          .ny_N    = ny_N,
                          .Ju0     = std::move(Ju0)};
    res.update_impl(ocp);
    return res;
}

template struct CyqloneStorage<double>;
#if BATMAT_WITH_SINGLE
template struct CyqloneStorage<float>;
#endif

} // namespace cyqlone
